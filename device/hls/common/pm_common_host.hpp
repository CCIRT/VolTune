// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

/**
 * Power Manager common data and functions.
 * This header file will be included from HLS codes and host codes.
 * When this header file is used in host codes,
 * `COMMON_NO_HLS` macro must be defined before including this header file.
 */
#ifndef DEVICE_HLS_COMMON_PM_COMMON_HOST_HPP__
#define DEVICE_HLS_COMMON_PM_COMMON_HOST_HPP__

#include <type_traits>
#include <cstdint>
#include <ostream>
#include <cmath>

#ifndef COMMON_NO_HLS
# include <ap_int.h>
# include <ap_axi_sdata.h>
# include <hls_stream.h>
# define __POWER_AP__ true
#else
 //
 // for host program (without Vitis HLS)
 //
# include <iostream>
#include "./for_host.hpp"
# define __POWER_AP__ false
#endif

#ifdef NAMESPACE_HLS
#define PM_COMMON_HLS_NS hls::
namespace hls{
#else
#define PM_COMMON_HLS_NS ::
#endif
namespace pmcommon {
// provide meta function
template<typename T> struct Range {};

template<> struct Range<uint16_t> {
  constexpr inline static uint16_t get(uint16_t value, size_t msb, size_t lsb) {
#pragma HLS inline
    const uint_fast16_t m = 0xFFFF >> (16 - msb - 1);
    const uint_fast16_t l = 0xFFFF << lsb;
    const uint_fast16_t ml = m & l;
    const uint_fast16_t v = value & ml;
    return (uint16_t)(v >> lsb);
  }
};

template<> struct Range<ap_uint<16>> {
  inline static ap_uint<16> get(const ap_uint<16>& value, size_t msb, size_t lsb) {
#pragma HLS inline
    return value(msb, lsb);
  }
};
}// end namespace common


namespace power_manager {

/**
 * Liner style 16bit Voltage value
 *
 * @param T uint16_t or ap_uint<16>
 */
template<typename T>
struct L16Base
{
  static constexpr uint16_t DIVI = 1 << 12;
  static constexpr float DIV = (float)DIVI;

  T value;
  inline L16Base(const T v): value(v) {
#pragma HLS inline
  }
  inline L16Base(const L16Base& v) : value(v.value) {
#pragma HLS inline
  }
  inline L16Base(L16Base&& v) : value(v.value) {
#pragma HLS inline
  }
  inline L16Base(float v) : value((uint16_t)(v* DIV)) {
#pragma HLS inline
  }

  inline operator T() const {
#pragma HLS inline
    return value;
  }
  inline operator float() const {
#pragma HLS inline
    return (uint16_t)value / DIV;
  }

  inline double as_double() const {
#pragma HLS inline
    return (float)*this;
  }

  inline uint16_t as_uint16_t() const {
#pragma HLS inline
    return (uint16_t)value;
  }
};

template<typename T>
std::ostream& operator<<(std::ostream& o, const L16Base<T>& v)
{
  const auto f = o.flags();
  o << "L16: 0x" << std::hex << v.as_uint16_t() << ", " << (float)v;
  o.flags(f);
  return o;
}


/**
 * Liner 11 style 16bit value
 *
 * @param T uint16_t or ap_uint<16>
 */
template<typename T>
struct L11Base
{
  static constexpr bool IS_AP = std::is_same<T, ap_uint<16>>::value;
  using exp_t = typename std::conditional<IS_AP, ap_int<5>, int16_t>::type;
  using mantissa_t = typename std::conditional<IS_AP, ap_int<11>, int16_t>::type;

  T value;
  inline L11Base(const T v): value(v) {
#pragma HLS inline
  }
  inline L11Base(const L11Base<T>& v) : value(v.value) {
#pragma HLS inline
  }
  inline L11Base(L11Base<T>&& v) : value(v.value) {
#pragma HLS inline
  }

  L11Base(float v): value(0x8000) {
#pragma HLS inline
    if (v != 0) {
      const auto sign = v < 0;
      const float max = sign ? 1024 : 1023;
      float uv = sign ? -v : v;
      int e = 0;

      // adjust uv and e
      if (uv > max) {
        for (; e < 16 && uv > max; e++, uv /= 2);

        if (e == 16 || uv > max) {
#ifdef COMMON_DISABLE_HLS
          std::cerr << "L11Base overflow detected: " << v << std::endl;
#endif
          uv = max;
          e = 15;
        }
      } else {
        while (std::ceil(uv) != uv && e != -16 && uv * 2 < max) {
          uv = uv * 2;
          e--;
        }
        if (uv < 1) {
#ifdef COMMON_DISABLE_HLS
          std::cerr << "L11Base underflow detected: " << v << std::endl;
#endif
          return;
        }
      }

      const T e16((uint16_t)(e << 11));
      const T uv16((uint16_t)((int16_t)(sign ? -uv : uv) & 0x7FF));
      value = e16 | uv16;
    }
  }

  inline operator T() const {
#pragma HLS inline
    return value;
  }

  inline exp_t exp() const {
#pragma HLS inline
    return exp_t(PM_COMMON_HLS_NS pmcommon::Range<T>::get(value, 15, 11));
  }

  inline mantissa_t mantissa() const {
#pragma HLS inline
    // TODO: fix int16_t (sign)
    return mantissa_t(PM_COMMON_HLS_NS pmcommon::Range<T>::get(value, 10, 0));
  }

  inline uint16_t as_uint16_t() const {
#pragma HLS inline
    return (uint16_t)value;
  }

  inline operator float() const {
#pragma HLS inline
    using ue_t = typename std::conditional<IS_AP, ap_uint<4>, uint16_t>::type;
    const auto m = mantissa();
    const auto e = exp();
    const bool es = (e & 0x10) == 0x10;
    const auto e30 = PM_COMMON_HLS_NS pmcommon::Range<T>::get(e, 3, 0);
    ue_t ue;
    if (es) ue = -e30;
    else   ue = e30;
    const auto uef = (float)(1 << ue);
    return es ? m / uef : m * uef;
  }
};

template<typename T>
std::ostream& operator<<(std::ostream& o, const L11Base<T>& v)
{
  const auto f = o.flags();
  o << "L11: 0x" << std::hex << v.as_uint16_t() << ", " << (float)v;
  o.flags(f);
  return o;
}



static constexpr int CountBitWidth = 32;
using PowerCountType = uint32_t;

/**
 * @param IntBitWidth integer part bit width of fixed point
 */
template<int IntBitWidth> struct PowerConstants {
  // power bit width
  static constexpr int BitWidth = 16;
  // integer part bit width of fixed point
  static constexpr int IntegerBitWidth = IntBitWidth;
  // decimal part bit width of fixed point
  static constexpr int N = BitWidth - IntegerBitWidth;
  // divide
  static constexpr float DIV = (float)(1L << N);

  // power * power bit width
  static constexpr int PowBitWidth = BitWidth * 2;
  // decimal part bit width of fixed point
  static constexpr int PowN = N * 2;
  // divide
  static constexpr float PowDIV = (float)(1L << PowN);

  // Σpower bit width
  static constexpr int SumBitWidth = BitWidth + CountBitWidth;
  // Σ(power * power) bit width
  static constexpr int PowSumBitWidth = PowBitWidth + CountBitWidth;

  // ap_uint<x>
  using ap_power = ap_uint<BitWidth>;
  using ap_pow = ap_uint<PowBitWidth>;
  using ap_sum = ap_uint<SumBitWidth>;
  using ap_powsum = ap_uint<PowSumBitWidth>;

  // primitive
  using power_t = uint16_t;
  using pow_t = uint32_t;
  using sum_t = uint64_t;
  using powsum_t = uint64_t;

};

// -------------------------------------------
// Device constants
// -------------------------------------------
using UCD9224EVMConst = PowerConstants<3>; // 3bit => 0 - 8[W]
using KC705Const = PowerConstants<3>;      // 3bit => 0 - 8[W]

#ifdef KC705
using DeviceConstants = KC705Const;
#elif defined(UCD9224EVM)
using DeviceConstants = UCD9224EVMConst;
#else
# error Device type not defined
#endif

template<typename PCONST, bool AP> struct Power;
template<typename PCONST, bool AP> struct PowerSum;
template<typename PCONST, bool AP> struct PowPower;
template<typename PCONST, bool AP> struct PowPowerSum;

/**
 * fixed point power value.
 * @param PCONST PowerConstants
 * @param AP     true: ap_uint, false: primitive
 */
template<typename PCONST, bool AP> struct Power
{
  using type = typename std::conditional<AP,
    typename PCONST::ap_power,
    typename PCONST::power_t>::type;
  using constructor_type = typename std::conditional<AP,
    typename PCONST::ap_power,
    typename PCONST::power_t>::type;


  using Pow = PowPower<PCONST, AP>;
  using pow_t = typename Pow::type;

  using Sum = PowerSum<PCONST, AP>;
  using sum_t = typename Sum::type;

  type value;

  inline Power(): value(0) {
#pragma HLS inline
  }

  inline Power(constructor_type t) : value(t) {
#pragma HLS inline
  }

  inline Power(const Power& s) : value(s.value) {
#pragma HLS inline
  }

  inline Power(const L11Base<type>& l11) : value() {
#pragma HLS inline
    int16_t m = l11.mantissa();
    if (m < 0) {
      m = -m;
    }
    const int8_t e = (int8_t)l11.exp() + PCONST::N;
    if (e < 0)
      value = (uint16_t)m >> -e;
    else
      value = (uint16_t)m << e;
  }

  inline Power& operator=(const Power& p) {
#pragma HLS inline
    value = p.value;
    return *this;
  }

  inline Power(float p): value((uint16_t)(p* PCONST::DIV)) {
#pragma HLS inline
  }

  inline void clear() {
#pragma HLS inline
    value = 0;
  }

  inline operator float() const {
#pragma HLS inline
    return value / PCONST::DIV;
  }

  inline operator uint16_t() const {
#pragma HLS inline
    return value;
  }

  inline double toDouble() const {
#pragma HLS inline
    return (double)value / (double)PCONST::DIV;
  }

  inline uint16_t getBinary() const {
#pragma HLS inline
    return value;
  }

  inline Pow pow() const {
#pragma HLS inline
    return Pow(pow_t(pow_t(value) * pow_t(value)));
  }

  inline Sum operator +(const Power& b) const {
#pragma HLS inline
    return Sum(sum_t(sum_t(value) + sum_t(b.value)));
  }

  inline bool operator <(const Power& b) const {
#pragma HLS inline
    return value < b.value;
  }
  inline bool operator <=(const Power& b) const {
#pragma HLS inline
    return value <= b.value;
  }
  inline bool operator ==(const Power& b) const {
#pragma HLS inline
    return value == b.value;
  }
  inline bool operator !=(const Power& b) const {
#pragma HLS inline
    return value != b.value;
  }
  inline bool operator >(const Power& b) const {
#pragma HLS inline
    return value > b.value;
  }
  inline bool operator >=(const Power& b) const {
#pragma HLS inline
    return value >= b.value;
  }
  inline void setMin(const Power& b) {
#pragma HLS inline
    if (b.value < value)
      value = b.value;
  }
  inline void setMax(const Power& b) {
#pragma HLS inline
    if (b.value > value)
      value = b.value;
  }

  inline operator Sum() const {
#pragma HLS inline
    return Sum(sum_t(value));
  }
};

/**
 * Power * Power.
 * @param PCONST PowerConstants
 * @param AP     true: ap_uint, false: primitive
 */
template<typename PCONST, bool AP> struct PowPower {
  using type = typename std::conditional<AP,
    typename PCONST::ap_pow,
    typename PCONST::pow_t>::type;

  using Sum = PowPowerSum<PCONST, AP>;
  using sum_t = typename Sum::type;

  type value;

  inline PowPower(): value(0) {
#pragma HLS inline
  }

  inline PowPower(type v) : value(v) {
#pragma HLS inline
  }

  inline PowPower(float v): value(v * PCONST::PowDIV) {
#pragma HLS inline
  }

  inline PowPower(const PowPower& s) : value(s.value) {
#pragma HLS inline
  }
  inline PowPower& operator=(const PowPower& s) {
#pragma HLS inline
    value = s.value;
    return *this;
  }

  inline operator float() const {
#pragma HLS inline
    return value / PCONST::PowDIV;
  }

  inline double toDouble() const {
#pragma HLS inline
    return (double)value / (double)PCONST::PowDIV;
  }

  inline uint16_t getBinary() const {
#pragma HLS inline
    return value;
  }

  inline void clear() {
#pragma HLS inline
    value = 0;
  }

  inline operator type() const {
#pragma HLS inline
    return value;
  }

  inline type get() const {
#pragma HLS inline
    return value;
  }

  inline operator Sum() const {
#pragma HLS inline
    return Sum(sum_t(value));
  }

  inline Sum operator +(const PowPower& b) const {
#pragma HLS inline
    return Sum(sum_t(sum_t(value) + sum_t(b.value)));
  }

  inline bool operator <(const PowPower& b) const {
#pragma HLS inline
    return value < b.value;
  }
  inline bool operator <=(const PowPower& b) const {
#pragma HLS inline
    return value <= b.value;
  }
  inline bool operator ==(const PowPower& b) const {
#pragma HLS inline
    return value == b.value;
  }
  inline bool operator !=(const PowPower& b) const {
#pragma HLS inline
    return value != b.value;
  }
  inline bool operator >(const PowPower& b) const {
#pragma HLS inline
    return value > b.value;
  }
  inline bool operator >=(const PowPower& b) const {
#pragma HLS inline
    return value >= b.value;
  }

};
/**
 * Σpower
 * @param PCONST PowerConstants
 * @param AP     true: ap_uint, false: primitive
 */
template<typename PCONST, bool AP> struct PowerSum
{
  using type = typename std::conditional<AP,
    typename PCONST::ap_sum,
    typename PCONST::sum_t>::type;

  using P = Power<PCONST, AP>;


  type value;
  inline PowerSum(): value(0) {
#pragma HLS inline
  }

  inline PowerSum(type v) : value(v) {
#pragma HLS inline
  }

  inline PowerSum(float v) : value(v* PCONST::DIV) {
#pragma HLS inline
  }

  inline PowerSum(const PowerSum& s) : value(s.value) {
#pragma HLS inline
  }

  inline void clear() {
#pragma HLS inline
    value = 0;
  }

  inline PowerSum& operator +=(const P& t2) {
#pragma HLS inline
    value += t2.value;
    return *this;
  }

  inline PowerSum operator +(const P& t2) const {
#pragma HLS inline
    return PowerSum(value + t2.value);
  }

  inline operator float() const {
#pragma HLS inline
    return value / PCONST::DIV;
  }

  inline double toDouble() const {
#pragma HLS inline
    return (double)value / (double)PCONST::DIV;
  }

  inline uint64_t getBinary() const {
#pragma HLS inline
    return value;
  }

  inline operator type() const {
#pragma HLS inline
    return value;
  }

  inline PowerSum& operator =(const type& v) const {
#pragma HLS inline
    value = v;
    return *this;
  }

  inline bool operator <(const PowerSum& b) const {
#pragma HLS inline
    return value < b.value;
  }
  inline bool operator <=(const PowerSum& b) const {
#pragma HLS inline
    return value <= b.value;
  }
  inline bool operator ==(const PowerSum& b) const {
#pragma HLS inline
    return value == b.value;
  }
  inline bool operator !=(const PowerSum& b) const {
#pragma HLS inline
    return value != b.value;
  }
  inline bool operator >(const PowerSum& b) const {
#pragma HLS inline
    return value > b.value;
  }
  inline bool operator >=(const PowerSum& b) const {
#pragma HLS inline
    return value >= b.value;
  }
};

/**
 * Σ(power * power)
 * @param PCONST PowerConstants
 * @param AP     true: ap_uint, false: primitive
 */
template<typename PCONST, bool AP> struct PowPowerSum
{
  using type = typename std::conditional<AP,
    typename PCONST::ap_powsum,
    typename PCONST::powsum_t>::type;

  using P = PowPower<PCONST, AP>;

  type value;
  inline PowPowerSum(): value(0) {
#pragma HLS inline
  }

  inline PowPowerSum(const PowPowerSum& s) : value(s.value) {
#pragma HLS inline
  }

  inline PowPowerSum(type v) : value(v) {
#pragma HLS inline
  }
  inline PowPowerSum(float v) : value(v* PCONST::PowDIV) {
#pragma HLS inline
  }
  inline PowPowerSum& operator =(const type& v) const {
#pragma HLS inline
    value = v;
    return *this;
  }

  inline void clear() {
#pragma HLS inline
    value = 0;
  }

  inline PowPowerSum& operator +=(const P& t2) {
#pragma HLS inline
    value += t2.value;
    return *this;
  }

  inline PowPowerSum operator +(const P& t2) const {
#pragma HLS inline
    return PowPowerSum(value + t2.value);
  }

  inline operator float() const {
#pragma HLS inline
    return value / PCONST::PowDIV;
  }

  inline double toDouble() const {
#pragma HLS inline
    return (double)value / (double)PCONST::PowDIV;
  }

  inline uint64_t getBinary() const {
#pragma HLS inline
    return value;
  }

  inline operator type() const {
#pragma HLS inline
    return value;
  }

  inline bool operator <(const PowPowerSum& b) const {
#pragma HLS inline
    return value < b.value;
  }
  inline bool operator <=(const PowPowerSum& b) const {
#pragma HLS inline
    return value <= b.value;
  }
  inline bool operator ==(const PowPowerSum& b) const {
#pragma HLS inline
    return value == b.value;
  }
  inline bool operator !=(const PowPowerSum& b) const {
#pragma HLS inline
    return value != b.value;
  }
  inline bool operator >(const PowPowerSum& b) const {
#pragma HLS inline
    return value > b.value;
  }
  inline bool operator >=(const PowPowerSum& b) const {
#pragma HLS inline
    return value >= b.value;
  }
};

using DevicePower = Power<DeviceConstants, __POWER_AP__>;
using DevicePowerSum = PowerSum<DeviceConstants, __POWER_AP__>;
using DevicePowPower = PowPower<DeviceConstants, __POWER_AP__>;
using DevicePowPowerSum = PowPowerSum<DeviceConstants, __POWER_AP__>;

// UCD9224EVM:
//  Address: open/open (104)
//    number : 0 => page 0
//    number : 1 => page 1
//
// KC705:
// | Number |   Name    | PMBus Address(Dec) | Page  |
// | :----: | :-------: | :----------------: | :---: |
// |   0    |  VCCINT   |         52         |   0   |
// |   1    |  VCCAUX   |         52         |   1   |
// |   2    |  VCC3V3   |         52         |   2   |
// |   3    |   VADF    |         52         |   3   |
// |   4    |  VCC2V5   |         53         |   0   |
// |   5    |  VCC1V5   |         53         |   1   |
// |   6    |  MGTAVCC  |         53         |   2   |
// |   7    |  MGTAVTT  |         53         |   3   |
// |   8    | ACCAUX_IO |         54         |   0   |
// |   9    |  VCCBRAM  |         54         |   1   |
// |   10   | MGTVCCAUX |         54         |   2   |
//

template<typename T>
uint8_t getLaneAddr(T lane) {
#pragma HLS inline
#ifdef UCD9224EVM
  // for UCD9224EVM evaluation board
  return 104; // * open open
#elif defined(KC705)
  // for KC705
  return lane <= 3 ? 52 :
    lane <= 7 ? 53 :
    lane <= 10 ? 54 : 0xFF;
#else
# error Device type not defined
#endif
}

template<typename T>
uint8_t getLanePage(T lane) {
#pragma HLS inline
#ifdef UCD9224EVM
  // for UCD9224EVM evaluation board
  return lane;
#elif defined(KC705)
  // for KC705
  return lane <= 3 ? (uint8_t)lane :
    lane <= 7 ? (uint8_t)lane - 4 :
    lane <= 10 ? (uint8_t)lane - 8 : 0;
#else
# error Device type not defined
#endif
}

template<typename T>
uint16_t getLaneInitVoltageBinary(T lane) {
#pragma HLS inline

#ifdef UCD9224EVM
  switch(lane) {
  case 0: return 10240; // 2.5[V]
  case 1: return 4915; // 1.2[V]
  }
  return 0;

#elif defined(KC705)
  switch(lane) {
  case 0: return 4096; // 1[V]
  case 1: return 7372; // 1.8[V]
  case 2: return 13516; // 3.3[V]
  case 3: return 10240; // 2.5[V]
  case 4: return 10240; // 2.5[V]
  case 5: return 6144; // 1.5[V]
  case 6: return 4096; // 1[V]
  case 7: return 4915; // 1.2[V]
  case 8: return 8192; // 2[V]
  case 9: return 4096; // 1[V]
  case 10: return 7372; // 1.8[V]
  }
  return 0;
#else
# error Device type not defined
#endif
}

template<typename T>
bool isValidLane(T lane) {
#pragma HLS inline
#ifdef UCD9224EVM

  // for UCD9224EVM evaluation board
  return lane <= 1;

#elif defined(KC705)

  // for KC705
# ifdef MGTAVCC_ONLY
  return lane == 6;
# else
  return lane <= 10;
# endif

#else
# error Device type not defined
#endif
}
#endif

}// namespace power_manager

#ifdef NAMESPACE_HLS
}
#endif

template<typename A, bool B>
std::ostream& operator<<(std::ostream& o, const PM_COMMON_HLS_NS power_manager::Power<A, B>& p)
{
  o << (float)p << "[W]";
  return o;
}

template<typename A, bool B>
std::ostream& operator<<(std::ostream& o, const PM_COMMON_HLS_NS power_manager::PowPower<A, B>& p)
{
  o << (float)p << "[W^2]";
  return o;
}

template<typename A, bool B>
std::ostream& operator<<(std::ostream& o, const PM_COMMON_HLS_NS power_manager::PowerSum<A, B>& p)
{
  o << (float)p << "[W]";
  return o;
}

template<typename A, bool B>
std::ostream& operator<<(std::ostream& o, const PM_COMMON_HLS_NS power_manager::PowPowerSum<A, B>& p)
{
  o << (float)p << "[W^2]";
  return o;
}
