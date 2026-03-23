// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef XSDB_HPP__
#define XSDB_HPP__

#include <xsdb_error.hpp>
#include <pipe.hpp>

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <memory>

#include <filesystem>

// ---------------------------------------------
// class define
// ---------------------------------------------
namespace xsdb {
class Targets;
class TargetDevice;
class Xsdb;
}

std::ostream& operator<<(std::ostream&, const xsdb::TargetDevice&);
std::ostream& operator<<(std::ostream& o, const xsdb::Targets& d);
// ---------------------------------------------

namespace xsdb {

/**
 * Represents each device information of the XSDB targets command.
 */
class TargetDevice {
public:
  /** xsdb jtag id.*/
  uint32_t jtag;
  /** xsdb device nmae*/
  std::string name;
  /** xsdb jtag is selected*/
  bool selected;
  /** child device size*/
  size_t lenght() const { return sub.size(); }
  TargetDevice& operator[](size_t i) {
    return *sub.at(i).get();
  }
  const TargetDevice& operator[](size_t i) const {
    return *sub.at(i).get();
  }
  TargetDevice(const std::string& text);
  TargetDevice(TargetDevice&& d):
    jtag(d.jtag)
    , name(std::move(d.name))
    , selected(d.selected)
    , indent(d.indent)
    , sub(std::move(d.sub))
    , parent(d.parent) {}

private:
  friend Targets;
  friend std::ostream& ::operator<<(std::ostream&, const TargetDevice&);
  uint32_t indent;
  std::vector<std::unique_ptr<TargetDevice>> sub;
  TargetDevice* parent;
};

/**
 * xsdb targets command result.
 */
class Targets {
public:
  Targets(): targets(), _parent(nullptr) {}

  /** size of child TargetDevice*/
  size_t length() const { return targets.size(); }

  TargetDevice& operator[](size_t i) {
    return *targets.at(i).get();
  }
  const TargetDevice& operator[](size_t i) const {
    return *targets.at(i).get();
  }

private:
  friend std::ostream& ::operator <<(std::ostream&, const xsdb::Targets&);
  friend Xsdb;
  void add(const std::string& line);
  std::vector<std::unique_ptr<TargetDevice>> targets;
  TargetDevice* _parent;
};



/**
 * XSDB wrapper class
 */
class Xsdb
{
public:
  /**
   * Launch xsdb process.
   */
  Xsdb(): Xsdb(nullptr) {}
  /**
   * Launch xsdb process.
   *
   * @param xsdbpath xsdb command path
   */
  Xsdb(const char* xsdbpath);
  /**
   * Launch xsdb process.
   *
   * @param xsdbpath xsdb command path
   */
  Xsdb(const std::string& xsdbpath): Xsdb(xsdbpath.c_str()) {}

  /**
   * Destroy the Xsdb object. close method is called automatically.
   */
  ~Xsdb();

  /**
   * whether the xsdb process is alive.
   */
  bool ok() const;
  /**
   * return ok()
   */
  operator bool() const { return ok(); }

  /**
   * Set command timeout mill sec.
   * When you set 0, timeout is disabled.
   * Default value is 0.
   *
   * @param millisec milli seconds
   */
  void setTimeout(uint64_t millisec) { _timeout = millisec; }

  /**
   * connect to hw_server. run `connect -url -port` command.
   *
   * @param url hw_server url
   * @param port hw_server port
   */
  void connect(const std::string& url, uint16_t port);
  /**
   * connect to hw_server. run `connect -url` command.
   *
   * @param url hw_server url
   */
  void connect(const std::string& url);
  /**
   * connect to localhost hw_server. run `connect` command.
   */
  void connect();

  /**
   * Get target list
   *
   * @return Targets
   */
  Targets targets();

  /**
   * set target. run `target jtag` command.
   *
   * @param jtag
   */
  void target(uint32_t jtag);

  /**
   * Write bitstream to target fpga
   *
   * @param path bitstream file path
   */
  void writeBitstream(const std::filesystem::path& path);

  /**
   * Close xsdb. (The destructor will call this method.)
   */
  void close();

  /**
   * Read a register
   *
   * @param addr the address of the register
   */
  uint32_t readRegister(uint64_t addr);

  /**
   * Write a value to a register.
   *
   * @param addr the address of the register
   * @param vlaue write value
   */
  void writeRegister(uint64_t addr, uint32_t value);

  /**
   * Read registers.
   *
   * @param n size of `addr` and `value`
   * @param addr addresses
   * @param [out] value output
   */
  void readRegisters(size_t n, const uint64_t *addr, uint32_t *value)
  {
    for (size_t i = 0; i < n; i++)
      value[i] = readRegister(addr[i]);
  }
  /**
   * Write registers
   *
   * @param n size of `addr` and `value`
   * @param addr addresses
   * @param value values
   */
  void writeRegisters(size_t n, const uint64_t *addr, const uint32_t *value)
  {
    for (size_t i = 0; i < n; i++)
      writeRegister(addr[i], value[i]);
  }

  template <size_t N>
  void readRegisters(const uint64_t (&addr)[N], uint32_t (&value)[N])
  {
    readRegisters(N, addr, value);
  }

  template <size_t N>
  void writeRegisters(const uint64_t (&addr)[N], const uint32_t (&value)[N])
  {
    writeRegisters(N, addr, value);
  }

  /**
   * Write cmd to the stdin of xsdb process.
   */
  void sendCommand(const std::string& cmd);

  /**
   * Write cmd to the stdin of xsdb process.
   */
  void sendCommand(const char* cmd_cstr);

  /**
   * Write cmd to the stdin of xsdb process.
   */
  void sendCommand(const char* cmd, size_t len);

  /**
   * Read line from the stdout of xsdb process.
   * @param [out] buffer output line
   * @return success or fail
   */
  bool readLine(std::string& buffer);

  /**
   * Read line from the stderr of xsdb process.
   * @param [out] buffer output line
   * @return success or fail
   */
  bool readErrorLine(std::string& buffer);

  /**
   * Debug option. show log when read/write register.
   *
   * @param on
   */
  void setLog(bool on) { _log = on; }


private:
  Xsdb(const Xsdb&) = delete;
  static constexpr size_t BUFFER_SIZE = 1024;
  bool _closed;
  Pipe _stdin;
  Pipe _stdout;
  Pipe _stderr;
#ifdef _WIN32
  PROCESS_INFORMATION _pi;
#else
  pid_t _pid;
#endif
  char _buffer[BUFFER_SIZE];
  size_t _bufsize;
  char _ebuffer[BUFFER_SIZE];
  size_t _ebufsize;
  uint64_t _timeout;
  bool _connected;
  uint32_t _target;
  bool _log;
};

} // namespace xsdb



#endif
