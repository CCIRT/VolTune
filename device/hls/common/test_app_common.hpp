// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

/**
 * Test Application common data and functions.
 * Don't include this file from host program.
 */
#ifndef DEVICE_HLS_COMMON_TEST_APP_COMMON_HPP__
#define DEVICE_HLS_COMMON_TEST_APP_COMMON_HPP__

#include <pm_common.hpp>

namespace test_app {

struct Cmd {
  static constexpr size_t TEST_PATTERN_WIDTH = 8;
  static constexpr size_t TEST_SIZE_WIDTH = 64;
  static constexpr size_t TEST_VOLTAGE_WIDTH = 16;
  static constexpr size_t I2C_CLOCK_SEL_WIDTH = 8;
  static constexpr size_t WIDTH = TEST_PATTERN_WIDTH
                                + TEST_SIZE_WIDTH
                                + TEST_VOLTAGE_WIDTH
                                + I2C_CLOCK_SEL_WIDTH;

  using type = ap_uint<WIDTH>;

  ap_uint<TEST_PATTERN_WIDTH>  test_pattern;
  ap_uint<TEST_SIZE_WIDTH>     test_size;
  ap_uint<TEST_VOLTAGE_WIDTH>  test_voltage;
  ap_uint<I2C_CLOCK_SEL_WIDTH> i2cClockSelect;

  inline Cmd(): test_pattern(0), test_size(0), test_voltage(0), i2cClockSelect(0) {
#pragma HLS inline
  }

  inline Cmd(ap_uint<TEST_PATTERN_WIDTH>  test_pattern,
             ap_uint<TEST_SIZE_WIDTH>     test_size,
             ap_uint<TEST_VOLTAGE_WIDTH>  test_voltage,
             ap_uint<I2C_CLOCK_SEL_WIDTH> i2cClockSelect):
    test_pattern(test_pattern), test_size(test_size), test_voltage(test_voltage), i2cClockSelect(i2cClockSelect) {
#pragma HLS inline
  }

  inline Cmd(const type& t) :
    test_pattern  (t(TEST_PATTERN_WIDTH - 1, 0)),
    test_size     (t(TEST_PATTERN_WIDTH + TEST_SIZE_WIDTH - 1,
                     TEST_PATTERN_WIDTH)),
    test_voltage  (t(TEST_PATTERN_WIDTH + TEST_SIZE_WIDTH + TEST_VOLTAGE_WIDTH - 1,
                     TEST_PATTERN_WIDTH + TEST_SIZE_WIDTH)),
    i2cClockSelect(t(WIDTH - 1,
                     TEST_PATTERN_WIDTH + TEST_SIZE_WIDTH + TEST_VOLTAGE_WIDTH))
  {
#pragma HLS inline
  }

  inline operator type() const {
#pragma HLS inline
    type t;
    t(TEST_PATTERN_WIDTH - 1, 0) = test_pattern;
    t(TEST_PATTERN_WIDTH + TEST_SIZE_WIDTH - 1,
      TEST_PATTERN_WIDTH) = test_size;
    t(TEST_PATTERN_WIDTH + TEST_SIZE_WIDTH + TEST_VOLTAGE_WIDTH - 1,
      TEST_PATTERN_WIDTH + TEST_SIZE_WIDTH) = test_voltage;
    t(WIDTH - 1,
      TEST_PATTERN_WIDTH + TEST_SIZE_WIDTH + TEST_VOLTAGE_WIDTH) = i2cClockSelect;
    return t;
  }
};

struct Ack {
  static constexpr size_t RESULT_WIDTH = 8;
  static constexpr size_t ERROR_BIT_CNT_WIDTH = 64;
  static constexpr size_t DATA_BYTE_CNT_WIDTH = 64;
  static constexpr size_t LATENCY_WIDTH = 32;
  static constexpr size_t WIDTH = RESULT_WIDTH
                                + ERROR_BIT_CNT_WIDTH
                                + DATA_BYTE_CNT_WIDTH
                                + LATENCY_WIDTH
                                + power_manager::Ack::bits::width;

  using type = ap_uint<WIDTH>;

  ap_uint<RESULT_WIDTH> result;
  ap_uint<ERROR_BIT_CNT_WIDTH> error_bit_cnt;
  ap_uint<DATA_BYTE_CNT_WIDTH> data_byte_cnt;
  ap_uint<LATENCY_WIDTH> latency;
  power_manager::Ack::type pmbus_ack;

  inline Ack():
    result(0),
    error_bit_cnt(0),
    data_byte_cnt(0),
    latency(0),
    pmbus_ack(power_manager::Ack()) {
#pragma HLS inline
  }

  inline Ack(ap_uint<RESULT_WIDTH> result):
    result(result),
    error_bit_cnt(0),
    data_byte_cnt(0),
    latency(0),
    pmbus_ack(power_manager::Ack()) {
#pragma HLS inline
  }

  inline Ack(int result):
    result(result),
    error_bit_cnt(0),
    data_byte_cnt(0),
    latency(0),
    pmbus_ack(power_manager::Ack()) {
#pragma HLS inline
  }

  inline Ack(ap_uint<RESULT_WIDTH> result,
             power_manager::Ack pmbus_ack):
    result(result),
    error_bit_cnt(0),
    data_byte_cnt(0),
    latency(0),
    pmbus_ack(pmbus_ack) {
#pragma HLS inline
  }

  inline Ack(ap_uint<RESULT_WIDTH> result,
             ap_uint<ERROR_BIT_CNT_WIDTH> error_bit_cnt,
             ap_uint<DATA_BYTE_CNT_WIDTH> data_byte_cnt,
             ap_uint<LATENCY_WIDTH> latency,
             power_manager::Ack pmbus_ack):
    result(result),
    error_bit_cnt(error_bit_cnt),
    data_byte_cnt(data_byte_cnt),
    latency(latency),
    pmbus_ack(pmbus_ack) {
#pragma HLS inline
  }

  inline Ack(const type& t) :
    result       (t(RESULT_WIDTH - 1, 0)),
    error_bit_cnt(t(RESULT_WIDTH + ERROR_BIT_CNT_WIDTH - 1,
                    RESULT_WIDTH)),
    data_byte_cnt(t(RESULT_WIDTH + ERROR_BIT_CNT_WIDTH + DATA_BYTE_CNT_WIDTH - 1,
                    RESULT_WIDTH + ERROR_BIT_CNT_WIDTH)),
    latency      (t(RESULT_WIDTH + ERROR_BIT_CNT_WIDTH + DATA_BYTE_CNT_WIDTH + LATENCY_WIDTH - 1,
                    RESULT_WIDTH + ERROR_BIT_CNT_WIDTH + DATA_BYTE_CNT_WIDTH)),
    pmbus_ack    (t(WIDTH - 1                                             ,
                    RESULT_WIDTH + ERROR_BIT_CNT_WIDTH + DATA_BYTE_CNT_WIDTH + LATENCY_WIDTH)) {
#pragma HLS inline
  }

  inline operator type() const {
#pragma HLS inline
    type t;
    t(RESULT_WIDTH - 1, 0) = result;
    t(RESULT_WIDTH + ERROR_BIT_CNT_WIDTH - 1,
      RESULT_WIDTH) = error_bit_cnt;
    t(RESULT_WIDTH + ERROR_BIT_CNT_WIDTH + DATA_BYTE_CNT_WIDTH - 1,
      RESULT_WIDTH + ERROR_BIT_CNT_WIDTH) = data_byte_cnt;
    t(RESULT_WIDTH + ERROR_BIT_CNT_WIDTH + DATA_BYTE_CNT_WIDTH + LATENCY_WIDTH - 1,
      RESULT_WIDTH + ERROR_BIT_CNT_WIDTH + DATA_BYTE_CNT_WIDTH) = latency;
    t(WIDTH - 1,
      RESULT_WIDTH + ERROR_BIT_CNT_WIDTH + DATA_BYTE_CNT_WIDTH + LATENCY_WIDTH) = pmbus_ack;
    return t;
  }
};

struct Cnt {
  static constexpr size_t WIDTH = Ack::ERROR_BIT_CNT_WIDTH
                                + Ack::DATA_BYTE_CNT_WIDTH;

  using type = ap_uint<WIDTH>;

  ap_uint<Ack::ERROR_BIT_CNT_WIDTH> error_bit_cnt;
  ap_uint<Ack::DATA_BYTE_CNT_WIDTH> data_byte_cnt;

  inline Cnt():
    error_bit_cnt(0),
    data_byte_cnt(0) {
#pragma HLS inline
  }

  inline Cnt(ap_uint<Ack::ERROR_BIT_CNT_WIDTH> error_bit_cnt,
             ap_uint<Ack::DATA_BYTE_CNT_WIDTH> data_byte_cnt):
    error_bit_cnt(error_bit_cnt),
    data_byte_cnt(data_byte_cnt) {
#pragma HLS inline
  }

  inline Cnt(const type& t) :
    error_bit_cnt (t(Ack::ERROR_BIT_CNT_WIDTH - 1, 0)),
    data_byte_cnt (t(WIDTH - 1, Ack::ERROR_BIT_CNT_WIDTH)) {
#pragma HLS inline
  }

  inline operator type() const {
#pragma HLS inline
    type t;
    t(Ack::ERROR_BIT_CNT_WIDTH - 1, 0)     = error_bit_cnt;
    t(WIDTH - 1, Ack::ERROR_BIT_CNT_WIDTH) = data_byte_cnt;
    return t;
  }
};

}

#endif
