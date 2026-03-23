// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef DEVICE_HLS_TEST_APP_BASE_HPP__
#define DEVICE_HLS_TEST_APP_BASE_HPP__

#include <_gmp_const.h>

#include <test_app_common.hpp>

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
);

#endif
