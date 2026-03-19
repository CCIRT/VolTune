#ifndef HOST_VOLTAGE_REGS_HPP__
#define HOST_VOLTAGE_REGS_HPP__

#include <cstdint>
#include <vector>
#include <ostream>
#include <pm_data.hpp>

struct Data {
  double voltage;
  uint32_t time;
  Data(double v, uint32_t t): voltage(v), time(t) {}
  Data(uint32_t t, double v): voltage(v), time(t) {}
  Data() = default;
};

inline std::ostream& operator <<(std::ostream& o, const Data& d)
{
  o << d.time << ", " << d.voltage;
  return o;
}

template<typename XSDB>
struct Regs {
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
  bool isDone() {
    return readRegister(0) & 2;
  }

  void run() {
    writeRegister(0, 1);
  }

  uint32_t getResult() {
    return readRegister(0x10);
  }

  bool hasError() {
    return getResult() != 0;
  }

  void setLane(uint32_t lane) {
    writeRegister(0x20, lane);
  }

  uint32_t getLane() {
    return readRegister(0x20);
  }

  void setInitV(double value) {
    const L16 v((float)value);
    writeRegister(0x30, v.as_uint16_t());
  }

  double getInitV() {
    const uint16_t v = readRegister(0x30);
    return (float)L16(v);
  }

  void setTargetV(double value) {
    const L16 v((float)value);
    writeRegister(0x40, v.as_uint16_t());
  }

  double getTargetV() {
    const uint16_t v = readRegister(0x40);
    return (float)L16(v);
  }

  void setUnderVoltage(double value) {
    const L16 v((float)value);
    writeRegister(0x70, v.as_uint16_t());
  }

  double getUnderVoltage() {
    const uint16_t v = readRegister(0x70);
    return (float)L16(v);
  }
  void setPowerGoodOnVoltage(double value) {
    const L16 v((float)value);
    writeRegister(0x80, v.as_uint16_t());
  }

  double getPowerGoodOnVoltage() {
    const uint16_t v = readRegister(0x80);
    return (float)L16(v);
  }
  void setPowerGoodOffVoltage(double value) {
    const L16 v((float)value);
    writeRegister(0x90, v.as_uint16_t());
  }

  double getPowerGoodOffVoltage() {
    const uint16_t v = readRegister(0x90);
    return (float)L16(v);
  }

  void setWaitTime(uint64_t count) {
    const uint32_t l = count;
    const uint32_t h = count >> 32;
    writeRegister(0x60, l);
    writeRegister(0x64, h);
  }

  void setWaitMillisec(uint64_t count) {
    setWaitTime(count * 100 * 1000);
  }

  uint64_t getWaitTime() {
    const uint64_t l = readRegister(0x60);
    const uint64_t h = readRegister(0x64);
    return l | (h << 32);
  }

  uint32_t getBufferSize() {
    return readRegister(0xA0);
  }

  void setI2C_100kHz() {
    writeRegister(0x50, 0);
  }

  void setI2C_400kHz() {
    writeRegister(0x50, 1);
  }

  void setI2C_1MHz() {
    writeRegister(0x50, 2);
  }

  uint32_t getI2CClk() {
    return readRegister(0x50);
  }

  std::vector<Data> getData(size_t size) {
    const auto s = getBufferSize();
    if (size > s)
      size = s;

    std::vector<Data> v;
    v.reserve(s);
    uint16_t v16;
    for (uint32_t i = 0; i < size; i++) {
      uint16_t value;
      if ((i&1) == 0) {
        const uint32_t v32 = readRegister(0x1000 + i * 2);
        value = v32;
        v16 = v32 >> 16;
      } else {
        value = v16;
      }
      const uint32_t time = readRegister(0x4000 + 4 * i);
      v.emplace_back(L16(value).as_double(), time);
    }
    return v;
  }

  std::vector<Data> getAllData() {
    return getData(getBufferSize());
  }

};

#endif
