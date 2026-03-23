// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef HLS_POWER_MANAGER_HPP__
#define HLS_POWER_MANAGER_HPP__
#include <_gmp_const.h>
#include <pm_common.hpp>

using lane_t = power_manager::lane_t;
using addr_t = power_manager::addr_t;
using Cmd = power_manager::Cmd;
using Ack = power_manager::Ack;
using cmd = Cmd::type;
using ack = Ack::type;

using L16 = power_manager::L16Base<ap_uint<16>>;
using L11 = power_manager::L11Base<ap_uint<16>>;

using PowerCountType = power_manager::PowerCountType;
using Power = power_manager::DevicePower;
using PowerSum = power_manager::DevicePowerSum;
using PowPower = power_manager::DevicePowPower;
using PowPowerSum = power_manager::DevicePowPowerSum;

enum ReadMode
{
  NoRead,
  ReadByte,
  ReadWord
};

struct PMBusCmd
{
  using type = ap_axiu<24, 9, 0, 0>;

  ap_uint<8> cmd;
  ap_uint<8> data1;
  ap_uint<8> data2;
  bool data1Valid;
  bool data2Valid;
  bool readByte;
  bool readWord;
  addr_t addr;

  inline operator type() const {
#pragma HLS inline
    ap_uint<24> d;
    d(7, 0) = cmd;
    d(15, 8) = data1;
    d(23, 16) = data2;
    ap_uint<3> keep;
    keep[0] = 1;
    keep[1] = data1Valid;
    keep[2] = data2Valid;
    ap_uint<9> user;
    user(6, 0) = addr;
    user[7] = readByte;
    user[8] = readWord;
    ap_uint<1> last(1);
    return { d, keep, keep, user, last, 0, 0 };
  }

  bool operator==(const type& t) const {
    type tt = *this;
    return tt.data == t.data && tt.keep == t.keep && tt.user == t.user;
  }
  bool operator!=(const type& t) const { return !operator==(t); }
};

struct PMBusAck
{
  using type = ap_axiu<16, 2, 0, 0>;

  ap_uint<16> value;
  ap_uint<2> error;
  PMBusAck() = default;
  PMBusAck(uint16_t v): value(v), error(0) {}
  PMBusAck(bool addrError, bool writeError): value(0), error((addrError ? 1 : 0) | (writeError ? 2 : 0)) {}

  inline PMBusAck(const type& a) :
    value(a.data),
    error(a.user)
  {
#pragma HLS inline
  }

  inline operator type() const {
#pragma HLS inline
    type v;
    v.data = value;
    v.user = error;
    return v;
  }

  bool operator==(const type& t) const {
    type tt = *this;
    return tt.data == t.data && tt.user == t.user;
  }
  bool operator!=(const type& t) const { return !operator==(t); }

  inline bool ok() const {
#pragma HLS inline
    return error == 0;
  }

  explicit inline operator bool() const {
#pragma HLS inline
    return ok();
  }

  inline bool isAddressError() const {
#pragma HLS inline
    return error[0];
  }

  inline bool isWriteError() const {
#pragma HLS inline
    return error[1];
  }

  inline ap_uint<8> getByte() const {
#pragma HLS inline
    return value(7, 0);
  }

  inline ap_uint<16> getWord() const {
#pragma HLS inline
    return value;
  }
};

using pmbus_cmd = PMBusCmd::type;
using pmbus_ack = PMBusAck::type;


void PowerManager(
  hls::stream<cmd>& cmd,
  hls::stream<ack>& ack,
  hls::stream<pmbus_cmd>& bus_cmd,
  hls::stream<pmbus_ack>& bus_ack,
  const bool cancel,
  bool& error,
  const counter_t& counter,
  bool& monitoring
);

// -------------- PMBUS commands --------------------

inline PMBusCmd sendByte(addr_t addr, uint8_t cmd) {
#pragma HLS inline
  return { cmd, 0, 0, false, false, false, false, addr };
}

inline PMBusCmd writeByte(addr_t addr, uint8_t cmd, uint8_t data1) {
#pragma HLS inline
  return { cmd, data1, 0, true, false, false, false, addr };
}

inline PMBusCmd writeWord(addr_t addr, uint8_t cmd, uint8_t data1, uint8_t data2) {
#pragma HLS inline
  return { cmd, data1, data2, true, true, false, false, addr };
}

inline PMBusCmd writeWord(addr_t addr, uint8_t cmd, uint16_t d) {
#pragma HLS inline
  return { cmd, d, d >> 8, true, true, false, false, addr };
}

inline PMBusCmd readByte(addr_t addr, uint8_t cmd) {
#pragma HLS inline
  return { cmd, 0, 0, false, false, true, false, addr };
}

inline PMBusCmd readWord(addr_t addr, uint8_t cmd) {
#pragma HLS inline
  return { cmd, 0, 0, false, false, true, true, addr };
}

constexpr uint8_t CMD_PAGE = 0;//byte
constexpr uint8_t CMD_OPERATION = 1;//byte
constexpr uint8_t CMD_CLEAR_FAULTS = 3;
constexpr uint8_t CMD_VOUT_COMMAND = 0x21;//word L16
constexpr uint8_t CMD_VOUT_MAX = 0x24; // word L16
constexpr uint8_t CMD_VOUT_MARGIN_HIGH = 0x25; // word L16
constexpr uint8_t CMD_VOUT_MARGIN_LOW = 0x26; // word L16
constexpr uint8_t CMD_STATUS_WORD = 0x79; // word
constexpr uint8_t CMD_STATUS_VOUT = 0x7A; // byte
constexpr uint8_t CMD_STATUS_IOUT = 0x7B; //byte
constexpr uint8_t CMD_READ_VOUT = 0x8B; // word L16
constexpr uint8_t CMD_READ_IOUT = 0x8C; // word L11
constexpr uint8_t CMD_READ_POUT = 0x96; // word L11

constexpr uint8_t CMD_VOUT_UV_WARN_LIMIT = 0x43;
constexpr uint8_t CMD_VOUT_UV_FAULT_LIMIT = 0x44;
constexpr uint8_t CMD_VOUT_UV_FAULT_RESPONSE = 0x45;

constexpr uint8_t CMD_POWER_GOOD_ON = 0x5E;
constexpr uint8_t CMD_POWER_GOOD_OFF = 0x5F;

inline PMBusCmd setPage(addr_t addr, uint8_t page) {
#pragma HLS inline
  return writeByte(addr, CMD_PAGE, page);
}

inline PMBusCmd getPage(addr_t addr) {
#pragma HLS inline
  return readByte(addr, CMD_PAGE);
}

inline PMBusCmd setOperation(addr_t addr, uint8_t op) {
#pragma HLS inline
  return writeByte(addr, CMD_OPERATION, op);
}

inline PMBusCmd getOperation(addr_t addr) {
#pragma HLS inline
  return readByte(addr, CMD_OPERATION);
}

inline PMBusCmd clearFault(addr_t addr) {
#pragma HLS inline
  return sendByte(addr, CMD_CLEAR_FAULTS);
}

inline PMBusCmd setVout(addr_t addr, const L16& l16) {
#pragma HLS inline
  return writeWord(addr, CMD_VOUT_COMMAND, l16.value);
}

inline PMBusCmd getVoutCommand(addr_t addr) {
#pragma HLS inline
  return readWord(addr, CMD_VOUT_COMMAND);
}

inline PMBusCmd getVout(addr_t addr) {
#pragma HLS inline
  return readWord(addr, CMD_READ_VOUT);
}

inline PMBusCmd getIout(addr_t addr) {
#pragma HLS inline
  return readWord(addr, CMD_READ_IOUT);
}

inline PMBusCmd getPout(addr_t addr) {
#pragma HLS inline
  return readWord(addr, CMD_READ_POUT);
}

inline PMBusCmd setVoutMax(addr_t addr, const L16& l16) {
#pragma HLS inline
  return writeWord(addr, CMD_VOUT_MAX, l16.value);
}

inline PMBusCmd getVoutMax(addr_t addr) {
#pragma HLS inline
  return readWord(addr, CMD_VOUT_MAX);
}

inline PMBusCmd setVoutMarginLow(addr_t addr, const L16& l16) {
#pragma HLS inline
  return writeWord(addr, CMD_VOUT_MARGIN_LOW, l16.value);
}

inline PMBusCmd getVoutMarginLow(addr_t addr) {
#pragma HLS inline
  return readWord(addr, CMD_VOUT_MARGIN_LOW);
}

inline PMBusCmd setVoutMarginHigh(addr_t addr, const L16& l16) {
#pragma HLS inline
  return writeWord(addr, CMD_VOUT_MARGIN_HIGH, l16.value);
}

inline PMBusCmd getVoutMarginHigh(addr_t addr) {
#pragma HLS inline
  return readWord(addr, CMD_VOUT_MARGIN_HIGH);
}

inline PMBusCmd getStatus(addr_t addr) {
#pragma HLS inline
  return readWord(addr, CMD_STATUS_WORD);
}

inline PMBusCmd getStatusVout(addr_t addr) {
#pragma HLS inline
  return readByte(addr, CMD_STATUS_VOUT);
}

inline PMBusCmd getStatusIout(addr_t addr) {
#pragma HLS inline
  return readByte(addr, CMD_STATUS_IOUT);
}

inline PMBusCmd setVoutUVWarnLimit(addr_t addr, const L16& l16)
{
#pragma HLS inline
  return writeWord(addr, CMD_VOUT_UV_WARN_LIMIT, l16.value);
}

inline PMBusCmd setVoutUVFaultLimit(addr_t addr, const L16& l16)
{
#pragma HLS inline
  return writeWord(addr, CMD_VOUT_UV_FAULT_LIMIT, l16.value);
}

inline PMBusCmd setPowerGoodOn(addr_t addr, const L16& l16)
{
#pragma HLS inline
  return writeWord(addr, CMD_POWER_GOOD_ON, l16.value);
}

inline PMBusCmd setPowerGoodOff(addr_t addr, const L16& l16)
{
#pragma HLS inline
  return writeWord(addr, CMD_POWER_GOOD_OFF, l16.value);
}


inline std::ostream& operator<<(std::ostream& o, const PMBusCmd& v) {
  const auto f = o.flags();
  o << std::hex;
  o << "{ Command: 0x" << (uint32_t)v.cmd << ", Address: " << (uint32_t)v.addr;
  if (v.data1Valid)
    o << ", data1: 0x" << (uint32_t)v.data1;

  if (v.data2Valid)
    o << ", data2: 0x" << (uint32_t)v.data2;
  if (v.readByte)
    o << ", Read: Byte";
  else if (v.readWord)
    o << ", Read: Word";
  else
    o << ", Read: None";
  o << " }";
  o.flags(f);
  return o;
}

#endif
