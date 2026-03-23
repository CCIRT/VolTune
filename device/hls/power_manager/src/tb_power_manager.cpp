// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "./power_manager.hpp"

#include <tbutil.hpp>

// to avoid cosim bug
static void callPowerManager(
  hls::stream<cmd>& cmd,
  hls::stream<ack>& ack,
  hls::stream<pmbus_cmd>& bus_cmd,
  hls::stream<pmbus_ack>& bus_ack,
  const bool cancel,
  bool& error,
  const counter_t& counter,
  bool& monitoring
) {
  PowerManager(cmd, ack, bus_cmd, bus_ack, cancel, error, counter, monitoring);
}

int main(int argc, char const* argv[])
{
  return RunTest(argc, argv);
}

// -------- Test Feature -----------------------------
struct PMTest: public tbutil::TestFeature {
  hls::stream<cmd> cmd_stream;
  hls::stream<ack> ack_stream;
  hls::stream<pmbus_cmd> pmbus_cmd_stream;
  hls::stream<pmbus_ack> pmbus_ack_stream;
  counter_t cnt;
  bool cancel;
  bool error;
  bool monitoring;

  virtual bool __before() {
    cancel = false;
    error = false;
    cnt = 0;
    return true;
  }

  virtual void __after() {
    AssertEmpty(cmd_stream);
    AssertEmpty(ack_stream);
    AssertEmpty(pmbus_cmd_stream);
    AssertEmpty(pmbus_ack_stream);
    AssertFalse(monitoring);
  }

  void run() {
    callPowerManager(cmd_stream, ack_stream, pmbus_cmd_stream, pmbus_ack_stream, cancel, error, cnt, monitoring);
    cnt += 100 * 1000;
  }

  void run(size_t n) {
    for (size_t i = 0; i < n; i++) {
      run();
    }
  }
};

// ---------------------------------------------------
// Tests
// ---------------------------------------------------


#ifndef UCD9224EVM
TEST(address) {
  AssertEqual(power_manager::getLaneAddr(0), 52);
  AssertEqual(power_manager::getLaneAddr(1), 52);
  AssertEqual(power_manager::getLaneAddr(2), 52);
  AssertEqual(power_manager::getLaneAddr(3), 52);
  AssertEqual(power_manager::getLaneAddr(4), 53);
  AssertEqual(power_manager::getLaneAddr(5), 53);
  AssertEqual(power_manager::getLaneAddr(6), 53);
  AssertEqual(power_manager::getLaneAddr(7), 53);
  AssertEqual(power_manager::getLaneAddr(8), 54);
  AssertEqual(power_manager::getLaneAddr(9), 54);
  AssertEqual(power_manager::getLaneAddr(10), 54);
}

TEST(page) {
  AssertEqual(power_manager::getLanePage(0), 0);
  AssertEqual(power_manager::getLanePage(1), 1);
  AssertEqual(power_manager::getLanePage(2), 2);
  AssertEqual(power_manager::getLanePage(3), 3);
  AssertEqual(power_manager::getLanePage(4), 0);
  AssertEqual(power_manager::getLanePage(5), 1);
  AssertEqual(power_manager::getLanePage(6), 2);
  AssertEqual(power_manager::getLanePage(7), 3);
  AssertEqual(power_manager::getLanePage(8), 0);
  AssertEqual(power_manager::getLanePage(9), 1);
  AssertEqual(power_manager::getLanePage(10), 2);
}

TEST(laneValid) {
# ifdef MGTAVCC_ONLY
  AssertFalse(power_manager::isValidLane(0));
  AssertFalse(power_manager::isValidLane(1));
  AssertFalse(power_manager::isValidLane(2));
  AssertFalse(power_manager::isValidLane(3));
  AssertFalse(power_manager::isValidLane(4));
  AssertFalse(power_manager::isValidLane(5));
  AssertTrue(power_manager::isValidLane(6));
  AssertFalse(power_manager::isValidLane(7));
  AssertFalse(power_manager::isValidLane(8));
  AssertFalse(power_manager::isValidLane(9));
  AssertFalse(power_manager::isValidLane(10));
  AssertFalse(power_manager::isValidLane(11));
# else
  AssertTrue(power_manager::isValidLane(0));
  AssertTrue(power_manager::isValidLane(1));
  AssertTrue(power_manager::isValidLane(2));
  AssertTrue(power_manager::isValidLane(3));
  AssertTrue(power_manager::isValidLane(4));
  AssertTrue(power_manager::isValidLane(5));
  AssertTrue(power_manager::isValidLane(6));
  AssertTrue(power_manager::isValidLane(7));
  AssertTrue(power_manager::isValidLane(8));
  AssertTrue(power_manager::isValidLane(9));
  AssertTrue(power_manager::isValidLane(10));
  AssertFalse(power_manager::isValidLane(11));
# endif
}
#endif

TEST(PowerTest) {
  // Power/ PowPower test
  L11 l00(0.0f);
  Power p00(l00);
  PowPower p00_2 = p00.pow();
  AssertEqual((float)l00, 0.0f);
  AssertEqual((float)p00, 0.0f);
  AssertEqual((float)p00_2, 0.0f);
  AssertEqual(p00, Power(0.0f));
  AssertEqual(p00_2, PowPower(0.0f));

  L11 l05(0.5f);
  Power p05(l05);
  PowPower p05_2 = p05.pow();
  AssertEqual((float)l05, 0.5f);
  AssertEqual((float)p05, 0.5f);
  AssertEqual((float)p05_2, 0.5f * 0.5f);
  AssertEqual(p05, Power(0.5f));
  AssertEqual(p05_2, PowPower(0.5f * 0.5f));


  L11 l10(1.0f);
  Power p10(l10);
  PowPower p10_2 = p10.pow();
  AssertEqual((float)l10, 1.0f);
  AssertEqual((float)p10, 1.0f);
  AssertEqual((float)p10_2, 1.0f * 1.0f);
  AssertEqual(p10, Power(1.0f));
  AssertEqual(p10_2, PowPower(1.0f));

  L11 l25(2.5f);
  Power p25(l25);
  PowPower p25_2 = p25.pow();
  AssertEqual((float)l25, 2.5f);
  AssertEqual((float)p25, 2.5f);
  AssertEqual((float)p25_2, 2.5f * 2.5f);
  AssertEqual(p25, Power(2.5f));
  AssertEqual(p25_2, PowPower(2.5f * 2.5f));


  const L11 lb3bb(ap_uint<16>(0xb3bb));
  float lb3bbf = (float)lb3bb;
  Power plb3bb(lb3bb);
  PowPower plb3bb_2 = plb3bb.pow();
  AssertEqual((float)plb3bb, lb3bbf);
  AssertEqual((float)plb3bb_2, lb3bbf * lb3bbf);


  // Sum test

  PowerSum psum;
  PowPowerSum psum2;
  psum.clear();
  psum2.clear();
  AssertEqual((float)psum, 0.0f);
  AssertEqual((float)psum2, 0.0f);

  psum += p00;
  psum2 += p00_2;
  AssertEqual((float)psum, 0.0f);
  AssertEqual((float)psum2, 0.0f);

  psum += p05;
  psum2 += p05_2;
  AssertEqual((float)psum, 0.5f);
  AssertEqual((float)psum2, 0.25f);

  psum += p10;
  psum2 += p10_2;
  AssertEqual((float)psum, 1.5f);
  AssertEqual((float)psum2, 1.25f);

  psum += p25;
  psum2 += p25_2;
  AssertEqual((float)psum, 4.0f);
  AssertEqual((float)psum2, 7.5f);

  psum += plb3bb;
  psum2 += plb3bb_2;
  AssertEqual((float)psum, 4.0f + lb3bbf);
  AssertEqual((float)psum2, 7.5f + lb3bbf * lb3bbf);

  psum.clear();
  psum2.clear();
  AssertEqual((float)psum, 0.0f);
  AssertEqual((float)psum2, 0.0f);
}

TEST_F(PMTest, monitoring) {
#ifdef UCD9224EVM
  lane_t lane = 0; // addr = 104, page = 0
  addr_t addr = 104;
  uint8_t page = 0;
#elif defined(KC705)
# ifdef MGTAVCC_ONLY
  lane_t lane = 6;
  addr_t addr = 53;
  uint8_t page = 2;
# else
  lane_t lane = 7; // addr = 53, page = 3
  addr_t addr = 53;
  uint8_t page = 3;
# endif
#else
# error Device type not defined
#endif
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(5);
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);

  cmd_stream.write(Cmd(Cmd::START_MONITORING, lane, 11));
  run(5);
  AssertEqual(ack_stream, { Ack(0) });
  run(4);
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEqual(pmbus_cmd_stream, { setPage(addr, page) });
  pmbus_ack_stream.write(PMBusAck(0));
  run(4);
  AssertEqual(pmbus_cmd_stream, { getPout(addr) });
  pmbus_ack_stream.write(PMBusAck(L11(2.5f).value));
  run(8);
  AssertEmpty(pmbus_ack_stream);
  AssertEqual(pmbus_cmd_stream, { getPout(addr) });
  pmbus_ack_stream.write(PMBusAck(L11(2.0f).value));
  cmd_stream.write(Cmd(Cmd::STOP_MONITORING));
  run(10);
  AssertEqual(ack_stream.size(), 1);
  Ack a(ack_stream.read());

  AssertEqual(a.value, Power(2.0f).value);
  AssertEqual(a.maxP, Power(2.5f));
  AssertEqual(a.count, 2);
  AssertEqual(a.sumP, PowerSum(4.5f));
  AssertEqual(a.sumP2, PowPowerSum(10.25f));
  // check do nothing
  run(10);
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);

  // Clear state and set undervoltage test
  cmd_stream.write(Cmd(Cmd::CLEAR_STATE, 0));
  run();
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  run();
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);
  run(4);
  AssertEmpty(pmbus_cmd_stream);
  AssertEqual(ack_stream, { Ack(0) });

  // check do nothing
  run(10);
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);

}

TEST_F(PMTest, readVout) {
#ifdef UCD9224EVM
  lane_t lane = 1; // addr = 104, page = 1
  addr_t addr = 104;
  uint8_t page = 1;
#elif defined(KC705)
  lane_t lane = 6;
  addr_t addr = 53;
  uint8_t page = 2;
#else
# error Device type not defined
#endif

  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  // send read vout
  cmd_stream.write(Cmd(Cmd::READ_VOUT, lane));
  run();
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(4);
  AssertEqual(pmbus_cmd_stream, { setPage(addr, page) });
  pmbus_ack_stream.write(PMBusAck(0));
  run();
  AssertEmpty(pmbus_ack_stream);
  AssertEmpty(pmbus_cmd_stream);
  run();
  AssertEqual(pmbus_cmd_stream, { getVout(addr) });
  pmbus_ack_stream.write(PMBusAck(20));
  run();
  AssertEmpty(pmbus_ack_stream);
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);
  run(4);
  AssertEqual(ack_stream, { Ack(20) });

  // send read vout
  // no page pmbus command
  cmd_stream.write(Cmd(Cmd::READ_VOUT, lane));
  run();
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(4);
  AssertEqual(pmbus_cmd_stream, { getVout(addr) });
  pmbus_ack_stream.write(PMBusAck(101));
  run();
  AssertEmpty(pmbus_ack_stream);
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);
  run(2);
  AssertEqual(ack_stream, { Ack(101) });


  // Clear state and set undervoltage test
  cmd_stream.write(Cmd(Cmd::CLEAR_STATE, 0));
  run();
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  run();
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);
  run(4);
  AssertEmpty(pmbus_cmd_stream);
  AssertEqual(ack_stream, { Ack(0) });
  cmd_stream.write(Cmd(Cmd::SET_UNDER_VOLTAGE, lane, 100));
  run();
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(4);
  AssertEqual(pmbus_cmd_stream, { setPage(addr, page) });
  pmbus_ack_stream.write(PMBusAck(0));
  run();
  AssertEmpty(pmbus_ack_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(3);
  AssertEqual(pmbus_cmd_stream, { setVoutUVWarnLimit(addr, L16(ap_uint<16>(100))) });
  pmbus_ack_stream.write(PMBusAck(0));
  run(3);
  AssertEqual(pmbus_cmd_stream, { setVoutUVFaultLimit(addr, L16(ap_uint<16>(100))) });
  pmbus_ack_stream.write(PMBusAck(0));
  run();
  AssertEmpty(pmbus_ack_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(4);
  AssertEqual(ack_stream, { Ack(0) });

  // PGood on
  cmd_stream.write(Cmd(Cmd::SET_PGOOD_ON_VOLTAGE, lane, 110));
  run();
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(4);
  AssertEqual(pmbus_cmd_stream, { setPowerGoodOn(addr, L16(ap_uint<16>(110))) });
  pmbus_ack_stream.write(PMBusAck(0));
  run();
  AssertEmpty(pmbus_ack_stream);
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);
  run(3);
  AssertEqual(ack_stream, { Ack(0) });

  // PGood off
  cmd_stream.write(Cmd(Cmd::SET_PGOOD_OFF_VOLTAGE, lane, 90));
  run();
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(4);
  AssertEqual(pmbus_cmd_stream, { setPowerGoodOff(addr, L16(ap_uint<16>(90))) });
  pmbus_ack_stream.write(PMBusAck(0));
  run();
  AssertEmpty(pmbus_ack_stream);
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);
  run(3);
  AssertEqual(ack_stream, { Ack(0) });


#ifdef UCD9224EVM
  lane = 0;
  page = 0;
#elif defined(KC705)
  lane = 7; // addr = 53, page = 3
  addr = 53;
  page = 3;
# ifdef MGTAVCC_ONLY
  // exit test
  if (lane != 6) return;
  // `if (lane != 6)`: to avoid compile error
# endif
#endif


  // send read vout
  // no page pmbus command
  cmd_stream.write(Cmd(Cmd::READ_VOUT, lane));
  run();
  AssertEmpty(ack_stream);
  AssertEmpty(cmd_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(5);
  AssertEqual(pmbus_cmd_stream, { setPage(addr, page) });
  pmbus_ack_stream.write(PMBusAck(0));
  run();
  AssertEmpty(pmbus_ack_stream);
  AssertEmpty(pmbus_cmd_stream);
  run(2);
  AssertEqual(pmbus_cmd_stream, { getVout(addr) });
  pmbus_ack_stream.write(PMBusAck(1020));
  run();
  AssertEmpty(pmbus_ack_stream);
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);
  run(4);
  AssertEqual(ack_stream, { Ack(1020) });

  // check do nothing
  run(10);
  AssertEmpty(pmbus_cmd_stream);
  AssertEmpty(ack_stream);
}
