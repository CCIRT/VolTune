// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef HOST_POWER_REGS_HPP__
#define HOST_POWER_REGS_HPP__

#include <cstdint>
#include <vector>
#include <ostream>
#include <pm_data.hpp>
#include <optional>
#include <trans_mode.hpp>

struct Data {
  uint64_t dataSize;
  uint64_t errorBitCount;
  double minPower;
  double maxPower;
  double averagePower;
  double dispersionPower;
  double standardDeviation;
  uint64_t latency;

  Data() = default;
  Data(
    uint64_t size,
    uint64_t ebc,
    double min,
    double max,
    double sum,
    double sum2,
    uint32_t count,
    uint64_t l
  ):
    dataSize(size * 8), //TODO: bit ? byte ?
    errorBitCount(ebc),
    minPower(min),
    maxPower(max),
    averagePower(sum / count),
    dispersionPower(sum2 / count - averagePower * averagePower),
    standardDeviation(std::sqrt(dispersionPower)),
    latency(l) {
  }

  double getLatencyNs(util::TransSpeed speed) const {
    return latency * (1000 * 1000 * 1000.0) / speed.clockFrequency;
  }

  double getErrorBitRate() const {
    return (double)errorBitCount / dataSize;
  }

};

inline std::ostream& operator <<(std::ostream& o, const Data& d)
{
  o << d.dataSize
    << ", " << d.errorBitCount
    << ", " << d.getErrorBitRate()
    << ", " << d.minPower
    << ", " << d.maxPower
    << ", " << d.averagePower
    << ", " << d.dispersionPower
    << ", " << d.standardDeviation;
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
  bool isDone() {
    // return readRegister(0) & 2;
    return readRegister(0x10000) == 4;
  }

  void run() {
    writeRegister(0x10000, 0);
    writeRegister(0x10000, 1);
    writeRegister(0x10000, 0);
  }

  void disableTest() {
    writeRegister(0x30000, 1);
  }

  void enableTest() {
    writeRegister(0x30000, 0);
  }

  void disablePMBus() {
    writeRegister(0x30008, 1);
  }

  void enablePMBus() {
    writeRegister(0x30008, 0);
  }

  // -----------------------------------------
  // config
  // -----------------------------------------

  void setTestParameter(uint64_t value, size_t index) {
    uint32_t l(value);
    uint32_t h(value >> 32);
    writeRegister(0x1000 + 0x10 * index, l);
    writeRegister(0x1004 + 0x10 * index, h);
  }

  uint64_t getTestSize() {
    uint64_t l = readRegister(0x2080);
    uint64_t h = readRegister(0x2084);
    return l | h << 32;
  }

  void setI2C_100kHz() {
    writeRegister(0x1020, 0);
  }

  void setI2C_400kHz() {
    writeRegister(0x1020, 1);
  }

  void setI2C_1MHz() {
    writeRegister(0x1020, 2);
  }

  void setTestVoltage(double v) {
    uint16_t binary = L16((float)v).as_uint16_t();
    writeRegister(0x20, binary);
  }
  // -----------------------------------------
  // Clock
  // -----------------------------------------
  void startClockCount() {
    writeRegister(0x20000, 0);
    writeRegister(0x20000, 1);
    writeRegister(0x20000, 0);
  }

  std::optional<uint32_t> getClockCount() {
    uint32_t v = readRegister(0x20008);
    if(v & 0x80000000) return v & ~uint32_t(0x80000000);
    return std::nullopt;
  }

  // -----------------------------------------
  // result
  // -----------------------------------------
  uint32_t getResult() {
    return readRegister(0x2000);
  }

  bool hasError() {
    return getResult() != 0;
  }

  uint64_t getErrorBitCount() {
    uint64_t l = readRegister(0x2010);
    uint64_t h = readRegister(0x2014);
    return l | h << 32;
  }

  uint64_t getLatency() {
    uint64_t l = readRegister(0x2020);
    uint64_t h = readRegister(0x2024);
    return l | h << 32;
  }

  double getMinPower() {
    Power p(uint16_t(readRegister(0x2030)));
    return p.toDouble();
  }

  double getMaxPower() {
    Power p(uint16_t(readRegister(0x2040)));
    return p.toDouble();
  }

  double getPowerSum() {
    uint64_t l = readRegister(0x2050);
    uint64_t h = uint64_t(readRegister(0x2054)) << 32;
    auto v = l | h;
    return PowerSum(v).toDouble();
  }

  double getPowPowerSum() {
    uint64_t l = readRegister(0x2060);
    uint64_t h = uint64_t(readRegister(0x2064)) << 32;
    auto v = l | h;
    return PowPowerSum(v).toDouble();
  }

  uint32_t getPowerCount() {
    return readRegister(0x2070);
  }

  Data getData() {
    return Data(
      getTestSize(),
      getErrorBitCount(),
      getMinPower(),
      getMaxPower(),
      getPowerSum(),
      getPowPowerSum(),
      getPowerCount(),
      getLatency());
  }

};

#endif
