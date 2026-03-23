// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <pm_common.hpp>
#include <led_common.hpp>

using Cmd = power_manager::Cmd;
using Ack = power_manager::Ack;
using LED = led::LED;
using addr_t = power_manager::addr_t;
using lane_t = power_manager::lane_t;
using cmd = Cmd::type;
using ack = Ack::type;

void EvmPowerReader (
  hls::stream<counter_t>& counter,
  hls::stream<bool>& clear_counter,
  hls::stream<cmd>& cmd,
  hls::stream<ack>& ack,
  LED::stream& led,
  // AXI-Lite registers
  ap_uint<8>& result,
  lane_t lane,
  ap_uint<2> i2cClkReg,
  uint16_t& min_power,
  uint16_t& max_power,
  uint64_t& sum_power,
  uint64_t& sum_power2,
  uint32_t& sum_count,
  ap_uint<2>& i2cClockSelect
) {
  // AXI stream
#pragma HLS INTERFACE mode=axis port=counter name=s_axis_counter register_mode=off
#pragma HLS INTERFACE mode=axis port=clear_counter name=m_axis_clear_counter
#pragma HLS INTERFACE mode=axis port=cmd name=m_axis_cmd
#pragma HLS INTERFACE mode=axis port=ack name=s_axis_ack
#pragma HLS INTERFACE mode=axis port=led name=m_axis_led


  // AXI Lite
#pragma HLS INTERFACE mode=s_axilite port=return     bundle=regs
#pragma HLS INTERFACE mode=s_axilite port=result     bundle=regs offset=0x10
#pragma HLS INTERFACE mode=s_axilite port=lane       bundle=regs offset=0x20
#pragma HLS INTERFACE mode=s_axilite port=i2cClkReg  bundle=regs offset=0x50
#pragma HLS INTERFACE mode=s_axilite port=min_power  bundle=regs offset=0x60
#pragma HLS INTERFACE mode=s_axilite port=max_power  bundle=regs offset=0x70
#pragma HLS INTERFACE mode=s_axilite port=sum_power  bundle=regs offset=0x80
#pragma HLS INTERFACE mode=s_axilite port=sum_power2 bundle=regs offset=0x90
#pragma HLS INTERFACE mode=s_axilite port=sum_count  bundle=regs offset=0xA0

#pragma HLS INTERFACE mode=ap_none port=i2cClockSelect

  i2cClockSelect = i2cClkReg;

  result = 0xFF;
  const lane_t target = lane;

  if (!power_manager::isValidLane(target)) {
    result = 0x1;
    led.write(LED(false, false));
    return;
  }

  clear_counter.write(true);
  led.write(LED(true, false));

  cmd.write(Cmd::clearState());
  if (!Ack(ack.read()).ok) {
    result = 0x2;
    led.write(LED(false, false));
    return;
  }

  cmd.write(Cmd::startMonitoring(target, 1));
  if (!Ack(ack.read()).ok) {
    result = 0x3;
    led.write(LED(false, false));
    return;
  }

  const auto start = counter.read();
  const counter_t timeout = 100000000L * 10;
  led.write(LED(false, false));
  for(;;) {
#pragma HLS loop_tripcount min=100000000 max=1000000000 avg=500000000
    const auto now = counter.read();
    if (now - start >= timeout) {
      break;
    }
  }

  cmd.write(Cmd::stopMonitoring());
  Ack a(ack.read());
  if (!a.ok) {
    result = 0x4;
    led.write(LED(false, false));
    return;
  }

  min_power = a.getMinPower().getBinary();
  max_power = a.getMaxPower().getBinary();
  sum_power = a.getPowerSum().getBinary();
  sum_power2= a.getPowPowerSum().getBinary();
  sum_count = a.getPowerCount();
  result = 0;

  led.write(LED(false, true));
}
