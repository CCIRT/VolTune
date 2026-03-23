// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "./vtest_manager.hpp"

#include <tbutil.hpp>

using L16 = power_manager::L16Base<ap_uint<16>>;

int main(int argc, char const* argv[])
{
  hls::stream<counter_t> counter;
  hls::stream<bool> clear_counter;
  hls::stream<cmd> cmd;
  hls::stream<ack> ack;
  LED::stream led;
  // AXI-Lite registers
  ap_uint<8> result;
  ap_uint<8> resultPort;
  lane_t lane = 1;
  ap_uint<16> initV = L16(1.0f);
  ap_uint<16> targetV = L16(0.5f);
  counter_t waitTime = 0;
  uint16_t bufferSize = 0;
  uint16_t bufferSizePort = 0;
  ap_uint<2> i2cClkReg;
  ap_uint<2> i2cClockSelect;
  ap_uint<32> timer[BUFFER_SIZE];
  ap_uint<16> voltage[BUFFER_SIZE];
  const ap_uint<16> uvVoltage = L16(0.25f);
  const ap_uint<16> pgoodOnV = L16(0.125f);
  const ap_uint<16> pgoodOffV = L16(0.0625f);;


  uint32_t t = 100 * 1000 * 1000 + 100;
  tbutil::addAll(counter, { 1, 10, 100, 1000 });
  counter.write(t);
  counter.write(t + 500);
  tbutil::addAll(ack, {
    Ack(0), // clear state
    Ack(0), // set uv
    Ack(0), // set pgood off
    Ack(0), // set pgood on
    Ack(0), // set voltage
    Ack(L16(1.1f).value),
    Ack(0), // set voltage
    });
  for (uint32_t i = 1; i < BUFFER_SIZE; i++) {
    counter.write(t + 1000 + 100 * i);
    ack.write(Ack(L16(0.5f + 0.5f * i / BUFFER_SIZE).value));
  }

  VTestManager(
    counter,
    clear_counter,
    cmd,
    ack,
    led,

    result,
    lane,
    initV,
    targetV,
    waitTime,
    bufferSize,
    i2cClkReg,
    uvVoltage,
    pgoodOnV,
    pgoodOffV,
    i2cClockSelect,
    timer,
    voltage
  );
  AssertPop(cmd, Cmd(Cmd::CLEAR_STATE, 0xFF));
  AssertPop(cmd, Cmd(Cmd::SET_UNDER_VOLTAGE, lane, uvVoltage));
  AssertPop(cmd, Cmd(Cmd::SET_PGOOD_OFF_VOLTAGE, lane, pgoodOffV));
  AssertPop(cmd, Cmd(Cmd::SET_PGOOD_ON_VOLTAGE, lane, pgoodOnV));
  AssertPop(cmd, Cmd(Cmd::SET_VOUT, lane, initV));
  AssertPop(cmd, Cmd(Cmd::READ_VOUT, lane));
  AssertPop(cmd, Cmd(Cmd::SET_VOUT, lane, targetV));
  AssertEqual(led, {
    LED(true, false),
    LED(false, true)
  });
  AssertEqual(timer[0], t + 500);
  AssertEqual(voltage[0], L16(1.1f).value);
  AssertEqual(clear_counter, { true });

  for (uint32_t i = 1; i < BUFFER_SIZE; i++) {
    AssertEqual(timer[i], t + 1000 + 100 * i);
    AssertEqual(voltage[i], L16(0.5f + 0.5f * i / BUFFER_SIZE).value);
    AssertPop(cmd, Cmd(Cmd::READ_VOUT, lane));
  }
  AssertEqual(bufferSize, BUFFER_SIZE);
  std::cout << "Test Done" << std::endl;
  return 0;
}
