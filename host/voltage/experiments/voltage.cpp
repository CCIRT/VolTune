#include "./helper.hpp"
#include "../src/regs.hpp"

// bitstream path
const char* file = "./bitstream/voltage/n000.bit";
constexpr uint8_t lane = 6;// 6: MGTAVCC, 7: MGTAVTT

int main(int argc, char const* argv[])
{
  try {
    std::cout << "Connect xsdb" << std::endl;
    std::cout.flush();
    xsdb::Xsdb x;
    x.setTimeout(20000);
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
      regs.setLane(lane);
      regs.setI2C_400kHz();
      // change 1[V]→0.5[V]
      regs.setInitV(1.0);
      regs.setTargetV(0.5);
      // warning
      regs.setPowerGoodOffVoltage(0.1);
      regs.setPowerGoodOnVoltage(0.2);
      regs.setUnderVoltage(0.1);

      regs.setWaitMillisec(200);
      std::cout << "Run." << std::endl;
      std::cout.flush();
      regs.run();
      sleep(1000);
      while(regs.isRun());
      std::cout << "Done." << std::endl;
      const auto ret = regs.getResult();
      if (ret != 0) {
        std::cout << "Test Error! Please power off the devices." << ret << std::endl;
        return 1;
      }
      auto d = regs.getData(200);
      std::cout << "---------------------------------" << std::endl;
      std::cout << "            Result" << std::endl;
      std::cout << "---------------------------------" << std::endl;
      for(auto& v: d) {
        std::cout << v << std::endl;
      }
      return 0;
    } else {
      std::cout << "Device is bussy" << std::endl;
      return 1;
    }
  } catch (xsdb::XsdbError& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
