// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "./ber_test_manager.hpp"

template<typename T, typename U>
static inline bool cmp_reg(const std::string& reg_name, const T reg_val, const U expect) {
  if (reg_val != expect) {
    std::cout << reg_name << " is not correct." << std::endl;
    std::cout << reg_name << " = " << reg_val << std::endl;
    return false;
  }
  return true;
}

static bool test_fixture(
  const uint64_t test_pattern,
  const uint64_t test_size,
  const uint64_t test_voltage,
  const uint64_t i2cClockSelect,
  const ap_uint<test_app::Ack::RESULT_WIDTH>        result,
  const ap_uint<test_app::Ack::ERROR_BIT_CNT_WIDTH> error_bit_cnt,
  const ap_uint<test_app::Ack::DATA_BYTE_CNT_WIDTH> data_byte_cnt,
  const ap_uint<test_app::Ack::LATENCY_WIDTH>       latency,
  const bool                          pmbus_ok,
  const power_manager::Ack::power_t   pmbus_min_power,
  const power_manager::Ack::power_t   pmbus_max_power,
  const power_manager::PowerCountType pmbus_sum_count,
  const power_manager::Ack::sum_t     pmbus_sum_power,
  const power_manager::Ack::powsum_t  pmbus_sum_power2) {

  // Output
  hls::stream<test_app::Cmd::type> cmd_out;
  hls::stream<bool> running_out;
  hls::stream<bool> error_out;
  uint64_t result_reg;
  uint64_t error_bit_cnt_reg;
  uint64_t data_byte_cnt_reg;
  uint64_t latency_reg;
  uint64_t min_power_reg;
  uint64_t max_power_reg;
  uint64_t sum_power_reg;
  uint64_t sum_power2_reg;
  uint64_t sum_count_reg;
  bool running;
  bool error;

  // Input
  hls::stream<test_app::Ack::type> ack_in;

  // Set values
  const auto ack = test_app::Ack(result, error_bit_cnt, data_byte_cnt, latency,
                                  power_manager::Ack(
                                    pmbus_ok,
                                    pmbus_min_power,
                                    pmbus_max_power,
                                    pmbus_sum_count,
                                    pmbus_sum_power,
                                    pmbus_sum_power2));
  ack_in.write(ack);

  // Run target
  BerTestManager(
    cmd_out,
    ack_in,
    running_out,
    error_out,
    test_pattern,
    test_size,
    test_voltage,
    i2cClockSelect,
    result_reg,
    error_bit_cnt_reg,
    data_byte_cnt_reg,
    latency_reg,
    min_power_reg,
    max_power_reg,
    sum_power_reg,
    sum_power2_reg,
    sum_count_reg);

  // Check reg values
  if (!cmp_reg("result_reg",        result_reg,        result))                       return false;
  if (!cmp_reg("error_bit_cnt_reg", error_bit_cnt_reg, error_bit_cnt))                return false;
  if (!cmp_reg("data_byte_cnt_reg", data_byte_cnt_reg, data_byte_cnt))                return false;
  if (!cmp_reg("latency_reg",       latency_reg,       latency))                      return false;
  if (!cmp_reg("min_power_reg",     min_power_reg,     pmbus_min_power.getBinary()))  return false;
  if (!cmp_reg("max_power_reg",     max_power_reg,     pmbus_max_power.getBinary()))  return false;
  if (!cmp_reg("sum_power_reg",     sum_power_reg,     pmbus_sum_power.getBinary()))  return false;
  if (!cmp_reg("sum_power2_reg",    sum_power2_reg,    pmbus_sum_power2.getBinary())) return false;
  if (!cmp_reg("sum_count_reg",     sum_count_reg,     pmbus_sum_count))              return false;

  // Check cmd_out
  if (cmd_out.size() != 1) {
    std::cout << "cmd_out is not asserted." << std::endl;
    std::cout << "cmd_out.size() = " << cmd_out.size() << std::endl;
    return false;
  } else {
    const auto cmd = cmd_out.read();
      if (cmd != test_app::Cmd::type(test_app::Cmd(
                                      test_pattern,
                                      test_size,
                                      test_voltage,
                                      i2cClockSelect))) {
      std::cout << "cmd is not correct" << std::endl;
      std::cout << "cmd = " << cmd << std::endl;
      return false;
    }
  }

  // Check running_out
  if (running_out.size() != 2) {
    std::cout << "running_out is not asserted." << std::endl;
    std::cout << "running_out.size() = " << running_out.size() << std::endl;
    return false;
  } else {
    while(running_out.size() != 0) {
      const auto running = running_out.read();
      const bool expect = (running_out.size() == 1)? true: false;
      if (running != expect) {
        std::cout << "running is not correct" << std::endl;
        std::cout << "running = " << running << std::endl;
        return false;
      }
    }
  }

  // Check error_out
  if (error_out.size() != 2) {
    std::cout << "error_out is not asserted." << std::endl;
    std::cout << "error_out.size() = " << error_out.size() << std::endl;
    return false;
  } else {
    while(error_out.size() != 0) {
      const auto error = error_out.read();
      if (error != false) {
        std::cout << "error is not correct" << std::endl;
        std::cout << "error = " << error << std::endl;
        return false;
      }
    }
  }

  return true;
}

int main(int argc, char const *argv[]) {

  // Scenario test
  if (!test_fixture(
          1, 1000, 0, 2, 0, 0x0, 1000, 0xF,
          true, 0x123, 0x456, 0x789, 0xABCDEF, 0x12345678)) {
    std::cout << "Scenario test failed." << std::endl;
    return 1;
  }

  return 0;
}
