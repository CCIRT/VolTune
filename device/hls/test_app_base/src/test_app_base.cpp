// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "./test_app_base.hpp"

void TestAppBase(
  hls::stream<ap_axiu<64,0,0,0>>& data_out,
  hls::stream<test_app::Cmd::type>& cmd_in,
  hls::stream<test_app::Ack::type>& ack_out,
  hls::stream<power_manager::Cmd::type>& pmbus_cmd_out,
  hls::stream<power_manager::Ack::type>& pmbus_ack_in,
  hls::stream<ap_uint<8>>& counter_clear_out,
  hls::stream<ap_uint<64>>& counter_val_in,
  hls::stream<ap_uint<1>>& reset_modules_out,
  hls::stream<test_app::Cnt::type>& cnt_in,
  const ap_uint<1> cancel_test,
  const ap_uint<test_app::Ack::LATENCY_WIDTH> latency_result,
  ap_uint<2>& i2cClockSelect
) {

#pragma HLS interface axis port=data_out
#pragma HLS interface axis port=cmd_in
#pragma HLS interface axis port=ack_out
#pragma HLS interface axis port=pmbus_cmd_out
#pragma HLS interface axis port=pmbus_ack_in
#pragma HLS interface axis port=counter_clear_out
#pragma HLS interface axis port=counter_val_in
#pragma HLS interface axis port=reset_modules_out
#pragma HLS interface axis port=cnt_in
#pragma HLS interface mode=ap_none port=cancel_test
#pragma HLS interface mode=ap_none port=latency_result
#pragma HLS interface mode=ap_none port=i2cClockSelect

  // Cancel
  bool cancel = false;

  // Error codes
  constexpr int SUCCESS = 0x0;
  constexpr int RESET_MODULES_OUT_FULL = 0x1;
  constexpr int MGTAVCC_CMD_OUT_FULL = 0x2;
  constexpr int MGTAVCC_ACK_NOT_OK = 0x3;
  constexpr int CLEAR_COUNTER_OUT_FULL = 0x4;
  constexpr int CLEAR_STATE_CMD_OUT_FULL = 0x5;
  constexpr int CLEAR_STATE_ACK_NOT_OK = 0x6;
  constexpr int START_MONITORING_CMD_OUT_FULL = 0x7;
  constexpr int START_MONITORING_ACK_NOT_OK = 0x8;
  constexpr int DATA_OUT_FULL = 0x9;
  constexpr int STOP_MONITORING_CMD_OUT_FULL = 0xA;
  constexpr int STOP_MONITORING_ACK_NOT_OK = 0xB;
  constexpr int TEST_CANCELED = 0xC;
  constexpr int RESET_MGTAVCC_CMD_OUT_FULL = 0xD;
  constexpr int RESET_MGTAVCC_ACK_NOT_OK = 0xE;

  // Setting
  const power_manager::lane_t target = 6;     // MGTAVCC
  const auto default_voltage = power_manager::getLaneInitVoltageBinary(target);
  const uint16_t monitoring_interval_ms = 1;  // Monitoring per 1 ms

  // Wait for cmd
  const test_app::Cmd cmd(cmd_in.read());
  const auto test_pattern = cmd.test_pattern;
  const auto test_size = cmd.test_size;
  const auto test_voltage = cmd.test_voltage;
  i2cClockSelect = cmd.i2cClockSelect;

  // Reset to modules
  reset_modules_out.write(0x1);
  while(reset_modules_out.full()) { /* Do nothing */ };
  if (reset_modules_out.full()) {ack_out.write(RESET_MODULES_OUT_FULL); return;}

  // Clear state of power_manager
  pmbus_cmd_out.write(power_manager::Cmd::clearState());
  while(pmbus_cmd_out.full()) { /* Do nothing */ };
  if (pmbus_cmd_out.full()) {ack_out.write(CLEAR_STATE_CMD_OUT_FULL); return;}
  else {
    const power_manager::Ack pmbus_ack = pmbus_ack_in.read();
    if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(CLEAR_STATE_ACK_NOT_OK, pmbus_ack)); return;}
    else { /* Success */}
  }

  // Set MGTAVCC
  const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH> lower_voltage_limit = 410; // 410 is 0.1 V
  if (test_voltage <= lower_voltage_limit) { /* Do not set voltage */}
  else {
    const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH> under_voltage          = test_voltage - lower_voltage_limit;
    const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH> power_good_on_voltage  = under_voltage;
    const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH> power_good_off_voltage = power_good_on_voltage - 41; // 41 is 0.01 V

    // Set Under Voltage
    pmbus_cmd_out.write(power_manager::Cmd::setUnderVoltage(target, under_voltage));
    while(pmbus_cmd_out.full()) { /* Do nothing */ };
    if (pmbus_cmd_out.full()) {ack_out.write(MGTAVCC_CMD_OUT_FULL); return;}
    else {
      const power_manager::Ack pmbus_ack = pmbus_ack_in.read();
      if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(MGTAVCC_ACK_NOT_OK, pmbus_ack)); return;}
      else { /* Success */}
    }

    // Set Power Good Off Voltage
    pmbus_cmd_out.write(power_manager::Cmd::setPowerGoodOff(target, power_good_off_voltage));
    while(pmbus_cmd_out.full()) { /* Do nothing */ };
    if (pmbus_cmd_out.full()) {ack_out.write(MGTAVCC_CMD_OUT_FULL); return;}
    else {
      const power_manager::Ack pmbus_ack = pmbus_ack_in.read();
      if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(MGTAVCC_ACK_NOT_OK, pmbus_ack)); return;}
      else { /* Success */}
    }

    // Set Power Good On Voltage
    pmbus_cmd_out.write(power_manager::Cmd::setPowerGoodOn(target, power_good_on_voltage));
    while(pmbus_cmd_out.full()) { /* Do nothing */ };
    if (pmbus_cmd_out.full()) {ack_out.write(MGTAVCC_CMD_OUT_FULL); return;}
    else {
      const power_manager::Ack pmbus_ack = pmbus_ack_in.read();
      if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(MGTAVCC_ACK_NOT_OK, pmbus_ack)); return;}
      else { /* Success */}
    }

    // Set Voltage
    pmbus_cmd_out.write(power_manager::Cmd::setVoltage(target, test_voltage));
    while(pmbus_cmd_out.full()) { /* Do nothing */ };
    if (pmbus_cmd_out.full()) {ack_out.write(MGTAVCC_CMD_OUT_FULL); return;}
    else {
      const power_manager::Ack pmbus_ack = pmbus_ack_in.read();
      if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(MGTAVCC_ACK_NOT_OK, pmbus_ack)); return;}
      else { /* Success */}
    }
  }

  // Wait 200 ms @ 125 MHz (10 Gbps)
  const ap_uint<64> wait_cycle = 125000000 / 5;
  counter_clear_out.write(0);
  while(counter_clear_out.full()) { /* Do nothing */ };
  if (counter_clear_out.full()) {ack_out.write(CLEAR_COUNTER_OUT_FULL); return;}
  else {
    while (counter_val_in.read() != 1000);
    while (counter_val_in.read() != wait_cycle);
  }

  // Start power monitoring
  pmbus_cmd_out.write(power_manager::Cmd::startMonitoring(target, monitoring_interval_ms));
  while(pmbus_cmd_out.full()) { /* Do nothing */ };
  if (pmbus_cmd_out.full()) {ack_out.write(START_MONITORING_CMD_OUT_FULL); return;}
  else {
    const power_manager::Ack pmbus_ack = pmbus_ack_in.read();
    if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(START_MONITORING_ACK_NOT_OK, pmbus_ack)); return;}
    else { /* Success */}
  }

  // Generate test data
  ap_uint<test_app::Cmd::TEST_SIZE_WIDTH> count = 0;
  ap_uint<test_app::Cmd::TEST_SIZE_WIDTH> size = 0;
  while (size < test_size) {
    if (cancel_test) {
      cancel = true;
      break;
    } else if (!data_out.full()){
      ap_axiu<64,0,0,0> data;
      switch (test_pattern) {
      case 0:
        // All Zero
        data.data = 0x0000000000000000;
        break;
      case 1:
        // All Hi
        data.data = 0xFFFFFFFFFFFFFFFF;
        break;
      default:
        // Count Up
        data.data = count++;
        break;
      }
      size += 8;
      if (size >= test_size) {
        data.last = 1;
      } else {
        data.last = 0;
      }
      data_out.write(data);
    }
  }

  // Cancel process
  if (cancel) {
    // Stop power monitoring
    power_manager::Ack pmbus_ack;
    pmbus_cmd_out.write(power_manager::Cmd::stopMonitoring());
    while(pmbus_cmd_out.full()) { /* Do nothing */ };
    if (pmbus_cmd_out.full()) {ack_out.write(STOP_MONITORING_CMD_OUT_FULL); return;}
    else {
      pmbus_ack = pmbus_ack_in.read();
      if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(STOP_MONITORING_ACK_NOT_OK, pmbus_ack)); return;}
      else { /* Success */}
    }

    // Reset to Default Voltage
    pmbus_cmd_out.write(power_manager::Cmd::setVoltage(target, default_voltage));
    while(pmbus_cmd_out.full()) { /* Do nothing */ };
    if (pmbus_cmd_out.full()) {ack_out.write(RESET_MGTAVCC_CMD_OUT_FULL); return;}
    else {
      const power_manager::Ack pmbus_ack = pmbus_ack_in.read();
      if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(RESET_MGTAVCC_ACK_NOT_OK, pmbus_ack)); return;}
      else { /* Success */}
    }

    // Wait 200 ms @ 125 MHz (10 Gbps)
    const ap_uint<64> wait_cycle = 125000000 / 5;
    counter_clear_out.write(0);
    while(counter_clear_out.full()) { /* Do nothing */ };
    if (counter_clear_out.full()) {ack_out.write(CLEAR_COUNTER_OUT_FULL); return;}
    else {
      while (counter_val_in.read() != 1000);
      while (counter_val_in.read() != wait_cycle);
    }

    // Read cnt result
    const test_app::Cnt cnt = cnt_in.read();
    const ap_uint<test_app::Ack::ERROR_BIT_CNT_WIDTH> error_bit_cnt_result = cnt.error_bit_cnt;
    const ap_uint<test_app::Ack::DATA_BYTE_CNT_WIDTH> data_byte_cnt_result = cnt.data_byte_cnt;

    // Export ack
    int test_result = TEST_CANCELED;
    ack_out.write(test_app::Ack(test_result, error_bit_cnt_result, data_byte_cnt_result, latency_result, pmbus_ack));
    return;
  }

  // Normal process
  while(data_out.full()) { /* Do nothing */ };
  if(data_out.full()) {ack_out.write(test_app::Ack(DATA_OUT_FULL)); return;}
  else {
    // Stop power monitoring
    power_manager::Ack pmbus_ack;
    pmbus_cmd_out.write(power_manager::Cmd::stopMonitoring());
    while(pmbus_cmd_out.full()) { /* Do nothing */ };
    if (pmbus_cmd_out.full()) {ack_out.write(STOP_MONITORING_CMD_OUT_FULL); return;}
    else {
      pmbus_ack = pmbus_ack_in.read();
      if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(STOP_MONITORING_ACK_NOT_OK, pmbus_ack)); return;}
      else { /* Success */}
    }

    // Reset to Default Voltage
    pmbus_cmd_out.write(power_manager::Cmd::setVoltage(target, default_voltage));
    while(pmbus_cmd_out.full()) { /* Do nothing */ };
    if (pmbus_cmd_out.full()) {ack_out.write(RESET_MGTAVCC_CMD_OUT_FULL); return;}
    else {
      const power_manager::Ack pmbus_ack = pmbus_ack_in.read();
      if (!pmbus_ack.ok) {ack_out.write(test_app::Ack(RESET_MGTAVCC_ACK_NOT_OK, pmbus_ack)); return;}
      else { /* Success */}
    }

    // Wait 200 ms @ 125 MHz (10 Gbps)
    const ap_uint<64> wait_cycle = 125000000 / 5;
    counter_clear_out.write(0);
    while(counter_clear_out.full()) { /* Do nothing */ };
    if (counter_clear_out.full()) {ack_out.write(CLEAR_COUNTER_OUT_FULL); return;}
    else {
      while (counter_val_in.read() != 1000);
      while (counter_val_in.read() != wait_cycle);
    }

    // Read cnt result
    const test_app::Cnt cnt = cnt_in.read();
    const ap_uint<test_app::Ack::ERROR_BIT_CNT_WIDTH> error_bit_cnt_result = cnt.error_bit_cnt;
    const ap_uint<test_app::Ack::DATA_BYTE_CNT_WIDTH> data_byte_cnt_result = cnt.data_byte_cnt;

    // Export ack
    int test_result = SUCCESS;
    ack_out.write(test_app::Ack(test_result, error_bit_cnt_result, data_byte_cnt_result, latency_result, pmbus_ack));
  }

  return;
}
