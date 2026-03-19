#define COMMON_NO_HLS
#include <pm_common_host.hpp>
#include "./helper.hpp"
#include <cmath>

// bitstream path
const char* file = "./bitstream/evm_power_test/top.bit";

using L16 = power_manager::L16Base<uint16_t>;
using Power = power_manager::DevicePower;
using PowPower = power_manager::DevicePowPower;
using PowerSum = power_manager::DevicePowerSum;
using PowPowerSum = power_manager::DevicePowPowerSum;

template<typename XSDB> struct Regs {
  static constexpr uint64_t OFFSET = 0;
  XSDB* x;
  Regs(XSDB& xsdb): x(&xsdb) {}

  inline uint32_t readRegister(uint64_t addr) {
    return x->readRegister(addr + OFFSET);
  }

  inline void writeRegister(uint64_t addr, uint32_t v) {
    x->writeRegister(addr + OFFSET, v);
  }

  bool isRun() {
    const auto x = readRegister(0);
    return (x & 3) == 1;
  }
  bool isIdle() {
    const auto x = readRegister(0);
    return (x & 4) != 0;
  }
  bool isDone() { return readRegister(0) & 2; }

  void run() { writeRegister(0, 1); }
  uint8_t getResult() { return (uint8_t)readRegister(0x10); }
  bool hasError() { return getResult() != 0; }
  void setLane(uint32_t lane) { writeRegister(0x20, lane); }
  uint32_t getLane() { return readRegister(0x20); }
  void setI2C_100kHz() { writeRegister(0x50, 0); }
  void setI2C_400kHz() { writeRegister(0x50, 1); }
  void setI2C_1MHz() { writeRegister(0x50, 2); }
  uint32_t getI2CClk() { return readRegister(0x50); }

  double getMinPower() {
    const uint16_t v = readRegister(0x60);
    return (float)Power(v);
  }

  double getMaxPower() {
    const uint16_t v = readRegister(0x70);
    return (float)Power(v);
  }

  PowerSum getPowerSum() {
    const uint64_t l = readRegister(0x80);
    const uint64_t h = readRegister(0x84);
    uint64_t v = l | (h << 32);
    return PowerSum(v);
  }

  PowPowerSum getPowPowerSum() {
    const uint64_t l = readRegister(0x90);
    const uint64_t h = readRegister(0x94);
    uint64_t v = l | (h << 32);
    return PowPowerSum(v);
  }

  uint32_t getCount() {
    return readRegister(0xA0);
  }
};

int main(int argc, char const* argv[])
{
  try {
    std::cout << "Connect xsdb" << std::endl;
    std::cout.flush();
    xsdb::Xsdb x;
    x.setTimeout(10000);
    x.connect();

    uint32_t djtag = getDeviceJTAG(x);
    if (djtag == 0xFFFFFFFF) {
      std::cout << "xc7z007s is not found." << std::endl;
      return 1;
    }
    std::cout << "Device JTAG: " << djtag << std::endl;
    x.target(djtag);
    std::cout << "Write bitstream" << std::endl;
    std::cout.flush();
    x.writeBitstream(file);

    uint32_t jtag = getJTAG2AXIJTAG(x);
    if (jtag == 0xFFFFFFFF) {
      std::cout << "JTAG2AXI is not found." << std::endl;
      return 1;
    }

    std::cout << "JTAG: " << jtag << std::endl;
    x.target(jtag);

    Regs<xsdb::Xsdb> regs(x);

    if (regs.isIdle()) {
      regs.setLane(0);
      regs.setI2C_400kHz();

      std::cout << "Run." << std::endl;
      std::cout.flush();
      regs.run();
      for(auto i = 1;i <= 10; i++) {
        sleep(1000);
        std::cout << "Sleep " << i << std::endl;
        std::cout.flush();
      }
      while(regs.isRun());
      std::cout << "Done." << std::endl;
      const auto ret = regs.getResult();
      if (ret != 0) {
        std::cout << "Test Error! Please power off the devices." << ret << std::endl;
        return 1;
      }

      double minP = regs.getMinPower();
      double maxP = regs.getMaxPower();
      uint32_t count = regs.getCount();
      double sumP = regs.getPowerSum().toDouble();
      double sumP2 = regs.getPowPowerSum().toDouble();
      double aveP = sumP / count;
      double aveP2 = sumP2 / count;
      double s2 = aveP2 - aveP * aveP;
      std::cout
      << "Min Power : " << minP << "[W]" << std::endl
      << "Max Power : " << maxP << "[W]" << std::endl
      << "count     : " << count << std::endl
      << "sum       : "  << sumP << std::endl
      << "Ave Power : " << aveP << "[W]" << std::endl
      << "dispersion: " << s2 << std::endl
      << "standard deviation: " << std::sqrt(s2) << std::endl;
      return 0;
    } else {
      std::cout << "Device is bussy" << std::endl;
      return 1;
    }
  }
  catch (xsdb::XsdbError& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
