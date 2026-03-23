// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "./power_manager.hpp"

#include <iostream>

constexpr uint8_t INIT_LANE = 0xF;

struct IntenalAck {
  static constexpr size_t DATA_WIDTH = 16;
  static constexpr size_t WIDTH = 1 + DATA_WIDTH;
  using data_t = ap_uint<DATA_WIDTH>;
  using type = ap_uint<WIDTH>;

  bool ok;
  data_t value; // L16 voltage

  inline IntenalAck(): ok(false), value(0) {
#pragma HLS inline
  }
  inline IntenalAck(bool o, const data_t& d) : ok(o), value(d) {
#pragma HLS inline
  }
  inline IntenalAck(const data_t& d) : ok(true), value(d) {
#pragma HLS inline
  }

  inline IntenalAck(const type& t) :
    ok(t[0]),
    value(t(WIDTH - 1, 1)) {
#pragma HLS inline
  }
  inline IntenalAck(uint16_t v) : ok(true), value(v) {
#pragma HLS inline
  }

  inline operator type() const {
#pragma HLS inline
    type t;
    t[0] = ok;
    t(WIDTH - 1, 1) = value;
    return t;
  }
  bool operator==(const type& t) const {
    type tt = *this;
    return tt == t;
  }
};

struct MonitoringCmd {
  bool start;
  lane_t lane;
  uint32_t interval;

  inline MonitoringCmd(): start(false), lane(0), interval(0) {}
  inline MonitoringCmd(lane_t l, uint32_t i) : start(true), lane(l), interval(i) {}
};

struct MonitoringAck {
  bool ok;
  PowerCountType count;
  Power minP;
  Power maxP;
  PowerSum sumP;
  PowPowerSum sumP2;
};

// --------------------------------------------------------------------------
// pmbus
// --------------------------------------------------------------------------
void PMBusManager(
  hls::stream<pmbus_cmd>& bus_cmd,
  hls::stream<pmbus_ack>& bus_ack,
  hls::stream<Cmd::type>& directcmd_req,
  hls::stream<IntenalAck::type>& directcmd_ack,
  hls::stream<lane_t>& monitoring_req,
  hls::stream<IntenalAck::type>& monitoring_ack,
  const bool cancel,
  bool& error
) {
#pragma HLS pipeline II=1


  static lane_t lane(INIT_LANE);
  static addr_t addr(0);
  static ap_uint<4> page(0);
  static bool fatal_error = false;
  static bool monitoring = false;
  error = fatal_error;

  enum State {
    IDLE = 0,
    Req = 1,
    Ret = 2
  };

  enum ReqState :bool{
    CHANGE_LANE = false,
    SEND_PMBUS_CMD = true,
  };

  static State state = IDLE;
  static ReqState rstate;
  static Cmd::type rdata;
  static bool waitAck;
  static bool dackError;
  static ap_uint<16> dackValue;
  static uint8_t commandState;
  static bool    commandDone;

#pragma HLS reset variable=state
#pragma HLS reset variable=lane
#pragma HLS reset variable=addr off
#pragma HLS reset variable=page off
#pragma HLS reset variable=rstate off
#pragma HLS reset variable=rdata off
#pragma HLS reset variable=waitAck off
#pragma HLS reset variable=dackError off
#pragma HLS reset variable=dackValue off
#pragma HLS reset variable=commandState off
#pragma HLS reset variable=commandDone off
  // WARNING:
  // An error that does not normally occur is a very dangerous condition.
  // The fatal error condition will not be cleared unless the FPGA is turned off.
#pragma HLS reset variable=fatal_error off

  switch (state) {
  case IDLE:
    if (!directcmd_req.empty()) {
      monitoring = false;
      rdata = directcmd_req.read();
      const Cmd cmd(rdata);
      if (cmd.cmd == Cmd::CLEAR_STATE) {
        state = Ret;
        dackError = fatal_error;
        dackValue = 0;
        // Don't clear fatal error
        lane = 0xFF;
        addr = 0;
        page = 0;
      } else if (fatal_error | cancel) {
        // error
        state = Ret;
        dackError = true;
        dackValue = 0;
      } else {
        state = Req;
        waitAck = false;
        commandState = 0;
        commandDone = false;
        rstate = (cmd.lane != lane)? CHANGE_LANE : SEND_PMBUS_CMD;
      }
    } else if (!monitoring_req.empty()) {
      monitoring = true;
      Cmd cmd(
        // Use the unused value in Req state as the value of the monitoring request.
        Cmd::CLEAR_STATE,
        monitoring_req.read(),
        0);

      if (fatal_error | cancel) {
        state = Ret;
        dackError = true;
        dackValue = 0;
      } else {
        rdata = cmd;
        state = Req;
        waitAck = false;
        commandState = 0;
        commandDone = false;
        if (cmd.lane != lane)
          rstate = CHANGE_LANE;
        else
          rstate = SEND_PMBUS_CMD;
      }
    }
    break;// end IDLE

  case Req: {
    const Cmd cmd(rdata);

    switch (rstate) {

    case CHANGE_LANE:
      if (!waitAck) {
        // if (!bus_cmd.full()) {
        const addr_t _addr(power_manager::getLaneAddr(cmd.lane));
        const ap_uint<4> _page(power_manager::getLanePage(cmd.lane));
        addr = _addr;
        page = _page;
        lane = cmd.lane;
        bus_cmd.write(setPage(_addr, _page));
        waitAck = true;
        // }
      } else {
        if (!bus_ack.empty()) {
          waitAck = false;
          PMBusAck ack(bus_ack.read());
          if (!ack) {
            // error
            fatal_error = true;
            state = Ret;
            dackError = true;
            dackValue = 0;
            commandDone = true;
          } else {
            rstate = SEND_PMBUS_CMD;
          }
        }
      }
      break; // end case CHANGE_LANE

    case SEND_PMBUS_CMD:
      if (!waitAck) {
        // if (!bus_cmd.full()) {
        waitAck = true;
        switch (cmd.cmd) {

          // Use the unused value in Req state as the value of the monitoring request.
        case Cmd::CLEAR_STATE: // power monitoring
          // read power
          commandDone = true;
          bus_cmd.write(getPout(addr));
          break; // end power monitoring

        case Cmd::SET_UNDER_VOLTAGE:
          switch (commandState) {
          case 0:
            bus_cmd.write(setVoutUVWarnLimit(addr, cmd.value));
            break;
          case 1:
            commandDone = true;
            bus_cmd.write(setVoutUVFaultLimit(addr, cmd.value));
            break;
          }
          break; // end Cmd::SET_UNDER_VOLTAGE

        case Cmd::SET_PGOOD_ON_VOLTAGE:
          commandDone = true;
          bus_cmd.write(setPowerGoodOn(addr, cmd.value));
          break;

        case Cmd::SET_PGOOD_OFF_VOLTAGE:
          commandDone = true;
          bus_cmd.write(setPowerGoodOff(addr, cmd.value));
          break;

        case Cmd::SET_VOUT:
          commandDone = true;
          bus_cmd.write(setVout(addr, cmd.value));
          break;

        case Cmd::READ_VOUT:
          commandDone = true;
          bus_cmd.write(getVout(addr));
          break;

        default:
          // error
          state = Ret;
          commandDone = true;
          dackError = true;
          dackValue = 0;
          break;
        }// end switch(cmd.cmd)
      // } // end if(!bus_cmd.full())
      } else {
        if (!bus_ack.empty()) {
          PMBusAck ack(bus_ack.read());
          waitAck = false;
          commandState++;
          if (!ack) {
            // error
            state = Ret;
            fatal_error = true;
            dackError = true;
            dackValue = 0;
            commandDone = true;
          } else if (commandDone) {
            state = Ret;
            dackError = false;
            dackValue = ack.getWord();
          }
        }
      } // end if(!waitAck) else
      break; // end case SEND_PMBUS_CMD:
    } // end switch(rstate)
  } break; // end Req

  case Ret:
    if (!monitoring /*&& !directcmd_ack.full()*/) {
      directcmd_ack.write(IntenalAck(!dackError, dackValue));
      state = IDLE;
    } else if (monitoring /*&& !monitoring_ack.full()*/) {
      monitoring_ack.write(IntenalAck(!dackError, dackValue));
      state = IDLE;
    }
    break; // end Ack

  }// end switch(state)
} // end PMBusManger

// --------------------------------------------------------------------------
// interface
// --------------------------------------------------------------------------
void commandHandler(
  hls::stream<cmd>& cmd,
  hls::stream<ack>& ack,
  hls::stream<Cmd::type>& directcmd_req,
  hls::stream<IntenalAck::type>& directcmd_ack,
  hls::stream<MonitoringCmd>& monitoring_cmd,
  hls::stream<MonitoringAck>& monitoring_ack
) {
#pragma HLS pipeline II=1
  enum State {
    IDLE,
    CHECK_LANE,
    DACK,
    MACK
  };
  static State state = IDLE;
  static Cmd c0;

#pragma HLS reset variable=state
#pragma HLS reset variable=c0 off
  switch (state) {
  case IDLE:
    if (!cmd.empty()) {
      const Cmd c(cmd.read());
      c0 = c;
      switch (c.cmd) {
      case Cmd::CLEAR_STATE:
        directcmd_req.write(Cmd(c));
        state = DACK;
        break;

      case Cmd::STOP_MONITORING:
        monitoring_cmd.write(MonitoringCmd());
        state = MACK;
        break;

      default:
        state = CHECK_LANE;
        break;
      }
    }
    break;

  case CHECK_LANE:
    if (power_manager::isValidLane(c0.lane)) {
      switch (c0.cmd) {
      case Cmd::SET_UNDER_VOLTAGE:
      case Cmd::SET_PGOOD_ON_VOLTAGE:
      case Cmd::SET_PGOOD_OFF_VOLTAGE:
      case Cmd::SET_VOUT:
      case Cmd::READ_VOUT:
        directcmd_req.write(c0);
        state = DACK;
        break;

      case Cmd::START_MONITORING: {
        lane_t mlane = c0.lane;
        uint16_t ms = c0.value == 0 ? (uint16_t)1 : (uint16_t)c0.value;
        uint32_t interval = (uint32_t)ms * 100 * 1000;
        monitoring_cmd.write(MonitoringCmd(mlane, interval));
        ack.write(Ack(0));
        state = IDLE;
      }  break;

      default:// invalid command
        ack.write(Ack());
      }
    } else {
      ack.write(Ack());
      state = IDLE;
    }
    break;

  case DACK:
    if (!directcmd_ack.empty()) {
      const IntenalAck a(directcmd_ack.read());
      ack.write(Ack(a.ok, a.value));
      state = IDLE;
    }
    break;

  case MACK:
    if (!monitoring_ack.empty()) {
      const auto m = monitoring_ack.read();
      ack.write(Ack(m.ok, m.minP, m.maxP, m.count, m.sumP, m.sumP2));
      state = IDLE;
    }
    break;
  }
}

// --------------------------------------------------------------------------
// monitoring
// --------------------------------------------------------------------------
void PowerMonitoring(
  hls::stream<MonitoringCmd>& cmd,
  hls::stream<MonitoringAck>& ack,
  hls::stream<lane_t>& monitoring_req,
  hls::stream<IntenalAck::type>& monitoring_ack,
  const counter_t& counter,
  bool& monitoring
) {
#pragma HLS pipeline II=1

  enum State {
    IDLE,
    REQ,
    ACK,
    ACK2
  };
  static State state = IDLE;
  static bool error = false;


  static lane_t lane;
  static ap_uint<COUNTER_BIT_WIDTH + 1> next = 0;
  static uint32_t  interval = 0;

  static bool first;
  static Power power0;
  static PowPower pow0;

  static PowerCountType count = 0;
  static Power minP;
  static Power maxP;
  static PowerSum psum;
  static PowPowerSum p2sum;

#pragma HLS reset variable=state
#pragma HLS reset variable=lane off
#pragma HLS reset variable=error off
#pragma HLS reset variable=count off
#pragma HLS reset variable=next off
#pragma HLS reset variable=interval off
#pragma HLS reset variable=psum off
#pragma HLS reset variable=p2sum off
#pragma HLS reset variable=minP off
#pragma HLS reset variable=maxP off
#pragma HLS reset variable=power0 off
#pragma HLS reset variable=pow0 off
#pragma HLS reset variable=first off

  monitoring = state != IDLE;
  const auto c0 = counter;

  switch (state) {
  case IDLE:
    if (!cmd.empty()) {
      const auto cm = cmd.read();
      if (cm.start) {
        count = 0;
        minP.clear();
        maxP.clear();
        psum.clear();
        p2sum.clear();
        lane = cm.lane;
        interval = cm.interval;
        first = true;
        state = error ? IDLE : REQ;
      } else {
        ack.write(MonitoringAck{
          !error,
          0,
          Power(), Power(), PowerSum(), PowPowerSum()
          });
      }
    }
    break; // end IDLE

  case REQ:
    if (!cmd.empty()) {
      const auto cm = cmd.read();
      if (cm.start) {
        count = 0;
        minP.clear();
        maxP.clear();
        psum.clear();
        p2sum.clear();
        lane = cm.lane;
        interval = cm.interval;
        first = true;
      } else {
        state = IDLE;
        ack.write(MonitoringAck{
          !error,
          count,
          minP, maxP, psum, p2sum });
      }
    } else if (!error /* && !monitoring_req.full() */) {
      ap_uint<COUNTER_BIT_WIDTH + 1> c;
      c(COUNTER_BIT_WIDTH - 1, 0) = c0;
      c[COUNTER_BIT_WIDTH] = (c0 <= 0xFFFFFFFF) ? ap_uint<1>(next[COUNTER_BIT_WIDTH]) : ap_uint<1>(0);

      if (first || next <= c) {
        monitoring_req.write(lane);
        ap_uint<COUNTER_BIT_WIDTH + 1> n = ap_uint<COUNTER_BIT_WIDTH + 1>(c0) + interval;
        next = n(COUNTER_BIT_WIDTH - 1, 0);
        first = false;
        state = ACK;
      }
    }

    break; // end case REQ:

  case ACK:
    if (!monitoring_ack.empty()) {
      const IntenalAck ack(monitoring_ack.read());
      if (!ack.ok) {
        state = REQ;
        error = true;
      } else {
        Power p(L11(ack.value));
        power0 = p;
        pow0 = p.pow();
        state = ACK2;// Note: Split process to achieve II=1.
      }
    }
    break; // end case ACK:

  case ACK2: // Note: Split process to achieve II=1.
    psum += power0;
    p2sum += pow0;
    if (count == 0) {
      minP = power0;
      maxP = power0;
    } else {
      minP.setMin(power0);
      maxP.setMax(power0);
    }
    count++;
    state = REQ;
    break; // end case ACK2

  default:
    state = IDLE;
    break;
  }
}

// --------------------------------------------------------------------------
// Top moudle
// --------------------------------------------------------------------------
void PowerManager(
  hls::stream<cmd>& cmd,
  hls::stream<ack>& ack,
  hls::stream<pmbus_cmd>& bus_cmd,
  hls::stream<pmbus_ack>& bus_ack,
  const bool cancel,
  bool& error,
  const counter_t& counter,
  bool& monitoring
) {
// #pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS DATAFLOW

#pragma HLS INTERFACE mode=axis port=cmd name=s_axis_cmd
#pragma HLS INTERFACE mode=axis port=ack name=m_axis_ack
#pragma HLS INTERFACE mode=axis port=bus_cmd name=m_axis_pmbus_cmd
#pragma HLS INTERFACE mode=axis port=bus_ack name=s_axis_pmbus_ack
#pragma HLS INTERFACE mode=ap_none port=counter
#pragma HLS INTERFACE mode=ap_none port=cancel
#pragma HLS INTERFACE mode=ap_none port=error
#pragma HLS INTERFACE mode=ap_none port=monitoring
#pragma HLS stable variable=error
#pragma HLS stable variable=counter
#pragma HLS stable variable=monitoring

  static hls::stream<Cmd::type> directcmd_req("directcmd_req");
#pragma HLS STREAM variable=directcmd_req depth=1
#pragma HLS reset variable=directcmd_req

  static hls::stream<IntenalAck::type> directcmd_ack("directcmd_ack");
#pragma HLS STREAM variable=directcmd_ack depth=1
#pragma HLS reset variable=directcmd_ack

  static hls::stream<lane_t> monitoring_ireq("monitoring_ireq");
#pragma HLS STREAM variable=monitoring_ireq depth=1
#pragma HLS reset variable=monitoring_ireq

  static hls::stream<IntenalAck::type> monitoring_iack("monitoring_iack");
#pragma HLS STREAM variable=monitoring_iack depth=2
#pragma HLS reset variable=monitoring_iack

  static hls::stream<MonitoringCmd> monitoring_cmd("monitoring_cmd");
#pragma HLS STREAM variable=monitoring_cmd depth=1
#pragma HLS reset variable=monitoring_cmd

  static hls::stream<MonitoringAck> monitoring_ack("monitoring_ack");
#pragma HLS STREAM variable=monitoring_ack depth=1
#pragma HLS reset variable=monitoring_ack

  PMBusManager(bus_cmd, bus_ack, directcmd_req, directcmd_ack, monitoring_ireq, monitoring_iack, cancel, error);
  PowerMonitoring(monitoring_cmd, monitoring_ack, monitoring_ireq, monitoring_iack, counter, monitoring);
  commandHandler(cmd, ack, directcmd_req, directcmd_ack, monitoring_cmd, monitoring_ack);
}
