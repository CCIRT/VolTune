// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "./vtest_manager.hpp"
#include <iostream>

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
#pragma HLS INTERFACE mode=s_axilite port=initV      bundle=regs offset=0x30
#pragma HLS INTERFACE mode=s_axilite port=targetV    bundle=regs offset=0x40
#pragma HLS INTERFACE mode=s_axilite port=i2cClkReg  bundle=regs offset=0x50
#pragma HLS INTERFACE mode=s_axilite port=waitTime   bundle=regs offset=0x60
#pragma HLS INTERFACE mode=s_axilite port=uvVoltage  bundle=regs offset=0x70
#pragma HLS INTERFACE mode=s_axilite port=pgoodOnV   bundle=regs offset=0x80
#pragma HLS INTERFACE mode=s_axilite port=pgoodOffV  bundle=regs offset=0x90
#pragma HLS INTERFACE mode=s_axilite port=bufferSize bundle=regs offset=0xA0
#pragma HLS INTERFACE mode=s_axilite port=voltage    bundle=regs offset=0x1000
#pragma HLS INTERFACE mode=s_axilite port=timer      bundle=regs offset=0x4000

#pragma HLS INTERFACE mode=ap_none port=i2cClockSelect

  i2cClockSelect = i2cClkReg;

  constexpr uint32_t DEFAULT_WAIT_TIME = 100 * 1000 * 1000;
  const auto wTime = waitTime == 0 ? counter_t(DEFAULT_WAIT_TIME) : waitTime;

  bufferSize = 0;
  result = 0xFF;
  counter_t start;

  for (auto i = 0; i < BUFFER_SIZE;i++) {
#pragma HLS unroll
    timer[i] = 0;
    voltage[i] = 0;
  }
  const lane_t target = lane;
  if (!power_manager::isValidLane(target)) {
    result = 0x1;
    led.write(LED(false, false));
    return;
  }

  clear_counter.write(true);
  led.write(LED(true, false));

  cmd.write(Cmd(Cmd::CLEAR_STATE, 0xFF));
  {
    Ack a(ack.read());
    if (!a.ok) {
      result = 0x2;
      led.write(LED(false, false));
      return;
    }
  }

  cmd.write(Cmd(Cmd::SET_UNDER_VOLTAGE, target, uvVoltage));
  {
    Ack a(ack.read());
    if (!a.ok) {
      result = 0x3;
      led.write(LED(false, false));
      return;
    }
  }

  cmd.write(Cmd(Cmd::SET_PGOOD_OFF_VOLTAGE, target, pgoodOffV));
  {
    Ack a(ack.read());
    if (!a.ok) {
      result = 0x4;
      led.write(LED(false, false));
      return;
    }
  }

  cmd.write(Cmd(Cmd::SET_PGOOD_ON_VOLTAGE, target, pgoodOnV));
  {
    Ack a(ack.read());
    if (!a.ok) {
      result = 0x5;
      led.write(LED(false, false));
      return;
    }
  }


  cmd.write(Cmd(Cmd::SET_VOUT, target, initV));
  {
    Ack a(ack.read());
    if (!a.ok) {
      result = 0x6;
      led.write(LED(false, false));
      return;
    }
  }

  start = counter.read();
wait_time:
  for (;;) {
    const auto now = counter.read();
    if (now - start >= wTime) {
      break;
    }
  }


  cmd.write(Cmd(Cmd::READ_VOUT, target));
  {
    Ack a(ack.read());
    if (!a.ok) {
      result = 0x7;
      led.write(LED(false, false));
      return;
    }

    const auto now = counter.read();

    timer[0] = now;
    voltage[0] = a.value;
    bufferSize = 1;
  }

  cmd.write(Cmd(Cmd::SET_VOUT, target, targetV));
  {
    Ack a(ack.read());
    if (!a.ok) {
      result = 0x8;
      led.write(LED(false, false));
      return;
    }
  }

collect:
  for (uint16_t idx = 1;idx < BUFFER_SIZE; idx++) {
#pragma HLS pipeline off
    cmd.write(Cmd(Cmd::READ_VOUT, target));
    {
      Ack a(ack.read());
      if (!a.ok) {
        result = 0x9;
        led.write(LED(false, false));
        return;
      }
      const auto now = counter.read();
      timer[idx] = now;
      voltage[idx] = a.value;
      bufferSize = idx + 1;
    }
  }
  result = 0;
  led.write(LED(false, true));
}
