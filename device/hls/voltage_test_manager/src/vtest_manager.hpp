// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef DEVICE_HLS_VTEST_MANGER_HPP__
#define DEVICE_HLS_VTEST_MANGER_HPP__
#include <_gmp_const.h>
#include <pm_common.hpp>
#include <led_common.hpp>

#ifndef MAX_BUFFER_SIZE
# define MAX_BUFFER_SIZE 2048
#endif

constexpr size_t BUFFER_SIZE = MAX_BUFFER_SIZE;

using Cmd = power_manager::Cmd;
using Ack = power_manager::Ack;
using LED = led::LED;
using addr_t = power_manager::addr_t;
using lane_t = power_manager::lane_t;
using cmd = Cmd::type;
using ack = Ack::type;

void VTestManager(
  hls::stream<counter_t>& counter,
  hls::stream<bool>& clear_counter,
  hls::stream<cmd>& cmd,
  hls::stream<ack>& ack,
  LED::stream& led,
  // AXI-Lite registers
  ap_uint<8>& result,
  lane_t lane,
  ap_uint<16> initV,
  ap_uint<16> targetV,
  counter_t waitTime,
  uint16_t& bufferSize,
  ap_uint<2> i2cClkReg,
  ap_uint<16> uvVoltage,
  ap_uint<16> pgoodOnV,
  ap_uint<16> pgoodOffV,
  ap_uint<2>& i2cClockSelect,
  ap_uint<32> timer[BUFFER_SIZE],
  ap_uint<16> voltage[BUFFER_SIZE]
);

#endif
