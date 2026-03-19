#ifndef DEVICE_HLS_BER_TEST_MANGER_HPP__
#define DEVICE_HLS_BER_TEST_MANGER_HPP__

#include <_gmp_const.h>

#include <test_app_common.hpp>

void BerTestManager(
  hls::stream<test_app::Cmd::type>& cmd_out,
  hls::stream<test_app::Ack::type>& ack_in,
  hls::stream<bool>& running_out,
  hls::stream<bool>& error_out,
  const uint64_t& test_pattern_reg,
  const uint64_t& test_size_reg,
  const uint64_t& test_voltage_reg,
  const uint64_t& i2cClockSelect_reg,
  uint64_t& result_reg,
  uint64_t& error_bit_cnt_reg,
  uint64_t& data_byte_cnt_reg,
  uint64_t& latency_reg,
  uint64_t& min_power_reg,
  uint64_t& max_power_reg,
  uint64_t& sum_power_reg,
  uint64_t& sum_power2_reg,
  uint64_t& sum_count_reg
);

#endif
