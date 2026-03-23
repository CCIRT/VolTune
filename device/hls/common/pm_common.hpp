// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

/**
 * Power Manager common data and functions.
 * Don't include this file from host program.
 */
#ifndef DEVICE_HLS_COMMON_PM_COMMON_HPP__
#define DEVICE_HLS_COMMON_PM_COMMON_HPP__
#include <iostream>

#include <ostream>
#include <cstdint>
#include <cstddef>
#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include "./pm_common_host.hpp"
#include "./packbits.hpp"

#ifndef COUNTER_BIT_WIDTH
# define COUNTER_BIT_WIDTH 64
#endif

using counter_t = ap_uint<COUNTER_BIT_WIDTH>;

namespace power_manager {
using addr_t = ap_uint<7>;

struct Cmd {
  static constexpr size_t CMD_WIDTH = 3;
  static constexpr size_t LANE_WIDTH = 4;
  static constexpr size_t DATA_WIDTH = 16;
  static constexpr size_t WIDTH = CMD_WIDTH + LANE_WIDTH + DATA_WIDTH;
  using cmd_t = ap_uint<CMD_WIDTH>;
  using lane_t = ap_uint<LANE_WIDTH>;
  using data_t = ap_uint<DATA_WIDTH>;
  using type = ap_uint<WIDTH>;
  /** clear internal state.(this command cannot clear error.)*/
  static constexpr uint8_t CLEAR_STATE = 0;
  static constexpr uint8_t SET_UNDER_VOLTAGE = 1;
  static constexpr uint8_t SET_PGOOD_ON_VOLTAGE = 2;
  static constexpr uint8_t SET_PGOOD_OFF_VOLTAGE = 3;
  static constexpr uint8_t SET_VOUT = 4;
  static constexpr uint8_t READ_VOUT = 5;
  static constexpr uint8_t START_MONITORING = 6;
  static constexpr uint8_t STOP_MONITORING = 7;

  static inline Cmd startMonitoring(lane_t l, uint16_t interval_ms) {
#pragma HLS inline
    return Cmd(START_MONITORING, l, interval_ms);
  }

  static inline Cmd stopMonitoring() {
#pragma HLS inline
    return Cmd(STOP_MONITORING, 0, 0);
  }

  static inline Cmd clearState() {
#pragma HLS inline
    return Cmd(CLEAR_STATE, 0xFF);
  }

  static inline Cmd setUnderVoltage(lane_t l, uint16_t v) {
#pragma HLS inline
    return Cmd(SET_UNDER_VOLTAGE, l, v);
  }

  static inline Cmd setPowerGoodOn(lane_t l, uint16_t v) {
#pragma HLS inline
    return Cmd(SET_PGOOD_ON_VOLTAGE, l, v);
  }

  static inline Cmd setPowerGoodOff(lane_t l, uint16_t v) {
#pragma HLS inline
    return Cmd(SET_PGOOD_OFF_VOLTAGE, l, v);
  }

  static inline Cmd setVoltage(lane_t l, uint16_t v) {
#pragma HLS inline
    return Cmd(SET_VOUT, l, v);
  }

  static inline Cmd getVoltage(lane_t l) {
#pragma HLS inline
    return Cmd(READ_VOUT, l);
  }





  cmd_t cmd;
  lane_t lane;
  data_t value; // L16 voltage or interval[ms]
  inline Cmd(): cmd(0), lane(0), value(0) {
#pragma HLS inline
  }
  inline Cmd(uint8_t c, uint8_t l) : cmd(c), lane(l), value(0) {
#pragma HLS inline
  }
  inline Cmd(uint8_t c, uint8_t l, uint16_t v) : cmd(c), lane(l), value(v) {
#pragma HLS inline
  }
  inline Cmd(const type& t) :
    cmd(t(CMD_WIDTH - 1, 0)),
    lane(t(CMD_WIDTH + LANE_WIDTH - 1, CMD_WIDTH)),
    value(t(WIDTH - 1, CMD_WIDTH + LANE_WIDTH))
  {
#pragma HLS inline
  }

  inline Cmd& operator=(const Cmd& c) {
    cmd = c.cmd;
    lane = c.lane;
    value = c.value;
    return *this;
  }

  inline operator type() const {
#pragma HLS inline
    type t;
    t(CMD_WIDTH - 1, 0) = cmd;
    t(CMD_WIDTH + LANE_WIDTH - 1, CMD_WIDTH) = lane;
    t(WIDTH - 1, CMD_WIDTH + LANE_WIDTH) = value;
    return t;
  }
};
using lane_t = Cmd::lane_t;

struct Ack {
  static constexpr size_t DATA_WIDTH = 16;
  using bits = common::PackBits<
    1, // 0:ok
    DATA_WIDTH, // 1:value or minP
    DeviceConstants::BitWidth,// 2:maxP
    CountBitWidth, // 3:count
    DeviceConstants::SumBitWidth,// 4:sumP
    DeviceConstants::PowSumBitWidth// 5:sumP2
  >;
  template<int I, typename T>
  inline static auto bitsOf(T& t) -> decltype(bits::template of<I>::get(t)) {
#pragma HLS inline
    return bits::template of<I>::get(t);
  }
  template<int I, typename T, typename T2>
  inline static void bitsSet(T& t, const T2& t2) {
#pragma HLS inline
    return bits::template of<I>::set(t, t2);
  }

  using type = typename bits::uint;
  using data_t = ap_uint<DATA_WIDTH>;
  using power_t = DevicePower;
  using sum_t = DevicePowerSum;
  using powsum_t = DevicePowPowerSum;

  bool ok;
  data_t value; // L16 voltage or minP
  // power_t minP; // merged
  power_t maxP;
  PowerCountType count;
  sum_t sumP;
  powsum_t sumP2;

  inline power_t getMinPower() const {
#pragma HLS inline
    return power_t(value);
  }
  inline power_t getMaxPower() const {
#pragma HLS inline
    return maxP;
  }
  inline PowerCountType getPowerCount() const {
#pragma HLS inline
    return count;
  }
  inline sum_t getPowerSum() const {
#pragma HLS inline
    return sumP;
  }
  inline powsum_t getPowPowerSum() const {
#pragma HLS inline
    return sumP2;
  }


  inline Ack(): ok(false), value(0),
    maxP(0), count(0), sumP(0), sumP2(0) {
#pragma HLS inline
  }
  inline Ack(bool o, const data_t& d) : ok(o), value(d),
    maxP(0), count(0), sumP(0), sumP2(0) {
#pragma HLS inline
  }
  inline Ack(const data_t& d) : ok(true), value(d),
    maxP(0), count(0), sumP(0), sumP2(0) {
#pragma HLS inline
  }
  inline Ack(const type& t) :
    ok(t[0]),
    value(bitsOf<1>(t)),
    maxP(power_t::type(bitsOf<2>(t))),
    count(bitsOf<3>(t)),
    sumP(sum_t::type(bitsOf<4>(t))),
    sumP2(powsum_t::type(bitsOf<5>(t))) {
#pragma HLS inline
  }

  inline Ack(uint16_t v): ok(true), value(v),
    maxP(0), count(0), sumP(0), sumP2(0) {
#pragma HLS inline
  }

  inline Ack(
    bool o,
    const power_t& min,
    const power_t& max,
    PowerCountType c,
    const sum_t& sP,
    const powsum_t& sP2
  ) :
    ok(o),
    value(min.getBinary()),
    maxP(max),
    count(c),
    sumP(sP),
    sumP2(sP2)
  {
#pragma HLS inline
  }

  inline operator type() const {
#pragma HLS inline
    type t;
    t[0] = ok;
    bitsSet<0>(t, ok);
    bitsSet<1>(t, value);
    bitsSet<2>(t, maxP.getBinary());
    bitsSet<3>(t, count);
    bitsSet<4>(t, sumP.getBinary());
    bitsSet<5>(t, sumP2.getBinary());
    return t;
  }

  bool operator==(const type& t) const {
    type tt = *this;
    return tt == t;
  }
};

}

inline std::ostream& operator<<(std::ostream& o, const power_manager::Ack& v) {
  if (v.ok)
    o << "{ OK: " << (uint16_t)v.value << "}";
  else
    o << "{ NG: " << (uint16_t)v.value << "}";
  return o;
}

inline std::ostream& operator<<(std::ostream& o, const power_manager::Cmd& v) {
  o << "{ command: " << (uint16_t)v.cmd << ", lane :" << (uint16_t)v.lane << ", data: " << (uint16_t)v.value << "}";
  return o;
}

#endif
