// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <cmath>
#include <iostream>
#include <fstream>
#include <xsdb.hpp>
#include <bitstream.hpp>
#include <kc705.hpp>
#include <utilfunc.hpp>
#include <string>
#include <sstream>

#include "./args.hpp"
#include "./config.hpp"
#include "./regs.hpp"

using Device = kc705;


/**
 * Write bitstream and set register
 *
 * @param args command line arguments
 * @param data current test data
 * @param xsdb XSDB
 * @param lastTestSpeed last write speed
 * @param busType HW or SW (to determine bitstream)
 * @param tx write tx side or rx side.
 * @param deviceIndex device index of xsdb device tree.(not JTAG number). 0: 1st, 1:2nd
 * @return true success
 * @return 0: ok, 1: skip bitstream, -1: error
 */
static int initTest(
  const Args& args,
  const TestData& data,
  xsdb::Xsdb& xsdb,
  int lastTestSpeed,
  util::PMBusType busType,
  util::TransMode mode,
  int deviceIndex)
{
  const auto dj = Device::getDevice(xsdb, deviceIndex);
  if (!dj) {
    std::cout << "[ERROR] Device not found." << std::endl;
    return -1;
  }
  if (args.showLog)
    std::cout << "[INFO] target " << (*dj) << std::endl;
  xsdb.target(*dj);

  Regs<xsdb::Xsdb> regs(xsdb);

  // update bitstream
  if (data.speed.speed != lastTestSpeed) {
    const auto name = util::getBitstreamName(busType, data.speed, args.externalClock, mode);
    const auto bitstream = util::findBitstream(args.bitstreamDirectory, name);

    if (bitstream.empty()) {
      std::cout << "[ERROR] line " << data.lineNumber << " bitstream (" << name << "*.bit) is not found." << std::endl;
      return 1;
    }
    if (args.showLog)
      std::cout << "[INFO] write bitstream : " << bitstream.generic_string() << std::endl;
    xsdb.writeBitstream(bitstream);
  }

  const auto jj = Device::getJTAG(xsdb);
  if (!jj) {
    const auto name = util::getBitstreamName(busType, data.speed, args.externalClock, mode);
    const auto bitstream = util::findBitstream(args.bitstreamDirectory, name);
    std::cout << "[ERROR] JTAG2AXI not found. Make sure Bitstream and design are correct. " << bitstream << std::endl;
    return -1;
  }
  xsdb.target(*jj);

  if (args.showLog) {
    std::cout << "[INFO] target " << (*jj) << std::endl;
    std::cout << "[INFO] init registers" << std::endl;
  }

  // Set registers
  bool d0 = deviceIndex == 0;
  double initv = d0 ? data.getD0InitVoltage() : data.getD1InitVoltage();
  double targetv = d0 ? data.getD0TargetVoltage() : data.getD1TargetVoltage();
  //   * Automatically calculates thresholds.
  //   * The coefficients (0.8,0.6,0.5) are not significant.
  double minv = std::min(initv, targetv);
  double uv = 0.3;
  double pgon = 0.3;
  double pgoff = 0.2;

  if (args.showLog) {
    std::cout << "[INFO] Lane:" << (uint32_t)data.lane << std::endl;
    std::cout << "[INFO] InitV:" << initv << std::endl;
    std::cout << "[INFO] TargetV:" << targetv << std::endl;
    std::cout << "[INFO] UV:" << uv << std::endl;
    std::cout << "[INFO] PGoodOn:" << pgon << std::endl;
    std::cout << "[INFO] PGoodOff:" << pgon << std::endl;
    std::cout << "[INFO] pmbus Speed:" << args.pmbusSpeed << std::endl;
  }

  regs.setLane((uint32_t)data.lane);
  regs.setInitV(initv);
  regs.setTargetV(targetv);
  regs.setUnderVoltage(uv);
  regs.setPowerGoodOffVoltage(pgoff);
  regs.setPowerGoodOnVoltage(pgon);
  switch (args.pmbusSpeed) {
  case 2:  regs.setI2C_1MHz(); break;
  case 1:  regs.setI2C_400kHz(); break;
  default: regs.setI2C_100kHz(); break;
  }

  if (args.showLog) {
    std::cout << "[INFO] done init registers" << std::endl;
  }

  return 0;
}

/**
 * run test manager
 * @param xsdb
 */
static bool runTest(const Args& args, xsdb::Xsdb& xsdb, int deviceIndex)
{
  const auto dj = Device::getDevice(xsdb, deviceIndex);
  if (!dj) {
    std::cout << "[ERROR] Fail to run test. Device not found." << std::endl;
    return false;
  }
  xsdb.target(*dj);
  const auto jj = Device::getJTAG(xsdb);
  if (!jj) {
    std::cout << "[ERROR] Fail to run test. JTAG2AXI not found." << std::endl;
    return false;
  }
  xsdb.target(*jj);

  if (args.showLog)
    std::cout << "[INFO] target " << *jj << " run." << std::endl;

  Regs<xsdb::Xsdb> regs(xsdb);
  regs.run();

  if (args.showLog)
    std::cout << "[INFO] target " << *jj << " run ok." << std::endl;
  return true;
}

/**
 * wait test manager done.
 * @param xsdb
 * @return true success
 * @return false fail
 */
static bool waitTest(const Args& args, xsdb::Xsdb& xsdb, int deviceIndex)
{
  const auto dj = Device::getDevice(xsdb, deviceIndex);
  if (!dj) {
    std::cout << "[ERROR] Fail to run test. Device not found." << std::endl;
    return false;
  }
  xsdb.target(*dj);
  const auto jj = Device::getJTAG(xsdb);
  if (!jj) {
    std::cout << "[ERROR] Fail to run test. JTAG2AXI not found." << std::endl;
    return false;
  }
  xsdb.target(*jj);

  Regs<xsdb::Xsdb> regs(xsdb);
  using clk = std::chrono::system_clock;
  const auto start = clk::now();
  while (regs.isRun()) {
    const auto n = clk::now() - start;
    if (n > std::chrono::seconds(5)) {
      std::cout << "[ERROR] Timeout. Device may be hang up." << std::endl;
      return false;
    }
  }
  const auto ret = regs.getResult();
  if (args.showLog) {
    std::cout << "[INFO] result =" << ret << std::endl;
  }
  return ret == 0;
}

static std::optional<std::vector<Data>> readData(xsdb::Xsdb& xsdb, int deviceIndex, int dataSize)
{
  const auto dj = Device::getDevice(xsdb, deviceIndex);
  if (!dj) {
    std::cout << "[ERROR] Fail to read data. Device not found." << std::endl;
    return std::nullopt;
  }

  xsdb.target(*dj);
  const auto jj = Device::getJTAG(xsdb);
  if (!jj) {
    std::cout << "[ERROR] Fail to read data. JTAG2AXI not found." << std::endl;
    return std::nullopt;
  }
  xsdb.target(*jj);

  Regs<xsdb::Xsdb> regs(xsdb);
  size_t s = dataSize <= 0 ? 2048 : dataSize < 100 ? 100 : dataSize;
  return regs.getData(s);
}

static size_t getSettlingTimeIndex(const std::vector<Data>& d, double errorMargin) {
  constexpr size_t N = 20;
  const auto s = d.size();

  // 1st step: detect error band
  //  Treat the average of the last N data points as the stable voltage.
  double sum = d[s - N].voltage;
  double max = sum;
  double min = sum;
  for (auto i = s - N + 1; i < s; i++) {
    const auto v = d[i].voltage;
    sum += v;
    max = std::max(max, v);
    min = std::min(min, v);
  }
  const auto voltage = sum / N;
  max = std::max(max, voltage * (1 + errorMargin));
  min = std::min(min, voltage * (1 - errorMargin));

  // 2nd step: detect settling time
  // Find the first index where the following consecutive N data points fall within the error range.
  size_t count = 0;
  size_t index = 0;
  for (auto i = index; i < s; i++) {
    const auto v = d[i].voltage;
    if (min <= v && v <= max) {
      count++;
      if (count == 1) index = i;
      else if (count == N) break;
    } else {
      count = 0;
    }
  }
  return index;
}

/**
 * Get the Settling Time (milli seconds)
 *
 * @param d data
 * @param clockHz Hz
 * @param index d[index]
 * @return [ms]
 */
static double getSettlingTime(const std::vector<Data>& d, double clockHz, size_t index)
{
  const auto t = d[index].time - d[0].time;
  return t / clockHz * 1000;
}

/** print title of the result csv file.*/
static void initResult(
  std::ostream& out
) {
  out << "Line, Speed, "
    "Device1, Device1 result[ms], Device1 initial voltage[V], Device1 target voltage[V], Device1 diff[V],"
    "Device2, Device2 result[ms], Device2 initial voltage[V], Device2 target voltage[V], Device2 diff[V]" << std::endl;
  out.flush();
}

/** print one line of the result csv file.*/
static void saveResult(
  const TestData& data,
  size_t testIndex,
  double time0,
  std::optional<double> time1,
  bool HW,
  std::ostream& out
) {
  out << data.lineNumber;

  if (data.speed.isNone()) {
    if (HW)
      out << ",HW,None";
    else
      out << ",SW,None";
  } else {
    out << "," << (HW ? "HW " : "SW ");
    if (data.speed.speed == 100) {
      out << "10g";
    } else if (data.speed.speed == 75) {
      out << "7.5g";
    } else if (data.speed.speed == 50) {
      out << "5g";
    } else if (data.speed.speed == 25) {
      out << "2.5g";
    } else {
      out << (data.speed.speed / 10) << "." << (data.speed.speed % 10) << "g";
    }
    switch (data.d0mode) {
    case util::TransMode::TX:
      out << ",Tx"; break;
    case util::TransMode::RX:
      out << ",Rx"; break;
    case util::TransMode::LOOP:
      out << ",Loop"; break;
    }
  }
  out << "," << time0
    << "," << data.getD0InitVoltage()
    << "," << data.getD0TargetVoltage()
    << "," << (data.getD0TargetVoltage() - data.getD0InitVoltage());
  if (time1) {
    out << "," << *time1;
    if (data.speed.isNone()) {
      out << ",None";
    } else {
      switch (data.d1mode) {
      case util::TransMode::TX:
        out << ",Tx"; break;
      case util::TransMode::RX:
        out << ",Rx"; break;
      case util::TransMode::LOOP:
        out << ",Loop"; break;
      }
    }
    out << "," << data.getD1InitVoltage()
      << "," << data.getD1TargetVoltage()
      << "," << (data.getD1TargetVoltage() - data.getD1InitVoltage());
  } else {
    out << ",--,--,--";
  }
  out << std::endl;
  out.flush();
}

/** write the monitoring data as csv file. */
static void saveMonitoringData(
  const Args& args,
  const TestData& data,
  const std::vector<Data>& d,
  int repeat,
  int devindex
) {
  // create file name
  std::stringstream ss;
  ss << "line" << data.lineNumber << "_";
  double s = data.speed.speed / 10.0;
  ss << "dev" << devindex;
  if (data.speed.isNone()) {
    ss << "_none_";
  } else {

    switch (devindex == 0 ? data.d0mode : data.d1mode) {
    case util::TransMode::TX:
      ss << "_tx_"; break;
    case util::TransMode::RX:
      ss << "_rx_"; break;
    case util::TransMode::LOOP:
      ss << "_loop_"; break;
    }

    ss << s << "GHz";
  }
  if (repeat != 0)
    ss << "(" << (repeat + 1) << ")";

  ss << ".csv";
  std::string fname = ss.str();

  const auto filePath = args.monitoringDataDirectory / fname;
  std::ofstream out(filePath);
  out << "\"time[ms]\",\"voltage[V]\"" << std::endl;
  for (auto x : d)
    out << (x.time / args.baseClockHz * 1000) << "," << x.voltage << std::endl;
}

/* run one test.
* @return 0: success, 1: skip, -1: error
*/
static int runTest(
  const Args& args,
  const TestData& data,
  xsdb::Xsdb& xsdb,
  size_t testIndex,
  int count,
  int lastSpeed,
  int repeat,
  util::PMBusType hw,
  std::ostream& resultOut)
{
  std::string lhead;
  {
    std::stringstream ss;
    ss << "[Test " << data.lineNumber;
    if (args.repeat > 1) {
      ss << "|" << repeat;
    }
    ss << "] ";
    lhead = ss.str();
  }

  std::cout << lhead << "Start";
  if (repeat == 0) std::cout << ".";
  else std::cout << "(" << (repeat + 1) << ").";
  std::cout << std::endl;
  std::cout.flush();
  // write bitstream, set register

  int i0 = initTest(args, data, xsdb, lastSpeed, hw, data.d0mode, 0);
  if (i0 == 1) {
    std::cout << lhead << "Skip. Bitstream not found." << std::endl;
    return 1;
  } else if (i0 < 0) {
    std::cout << lhead << "Faild." << std::endl;
    return -1;
  }
  if (count == 2) {
    int i1 = initTest(args, data, xsdb, lastSpeed, hw, data.d1mode, 1);
    if (i1 == 1) {
      std::cout << lhead << "Skip. Bitstream not found." << std::endl;
      return 1;
    } else if (i1 < 0) {
      std::cout << lhead << "Faild." << std::endl;
      return -1;
    }
  }

  // run
  if (!runTest(args, xsdb, 0)) {
    std::cout << lhead << "Faild to run. Dev0" << std::endl;
    return -1;
  }
  if (count == 2) {
    if (!runTest(args, xsdb, 1)) {
      std::cout << lhead << "Faild to run. Dev1" << std::endl;
      return -1;
    }
  }

  // wait
  if (!waitTest(args, xsdb, 0)) {
    std::cout << lhead << "Faild to wait. Dev0" << std::endl;
    return -1;
  }
  if (count == 2) {
    if (!waitTest(args, xsdb, 1)) {
      std::cout << lhead << "Faild to wait. Dev1" << std::endl;
      return -1;
    }
  }

  // get data and show result
  const auto data0 = readData(xsdb, 0, args.readDataSize);
  if (!data0) {
    std::cout << lhead << "Register read error." << std::endl;
    return -1;
  }
  const auto index0 = getSettlingTimeIndex(*data0, args.errorMargin);
  const auto time0 = getSettlingTime(*data0, args.baseClockHz, index0);


  std::cout << lhead << "Result: "
    << time0 << " [ms], ";
  double speed = data.speed.speed / 10.0;

  std::cout << "Dev1";
  if (!data.speed.isNone()) {
    switch (data.d0mode) {
    case util::TransMode::TX:
      std::cout << " Tx"; break;
    case util::TransMode::RX:
      std::cout << " Rx"; break;
    case util::TransMode::LOOP:
      std::cout << " Loop"; break;
    }
    std::cout << speed << " GHz";
  }
  std::cout << ", " << data.getD0InitVoltage() << "[V] -> " << data.getD0TargetVoltage() << "[V]" << std::endl;
  std::cout.flush();
  saveMonitoringData(args, data, *data0, repeat, 0);

  std::optional<double> time1Opt = std::nullopt;


  if (count == 2) {
    const auto data1 = readData(xsdb, 1, args.readDataSize);
    if (!data1) {
      std::cout << lhead << "Register read error." << std::endl;
      return -1;
    }
    const auto index1 = getSettlingTimeIndex(*data1, args.errorMargin);
    const auto time1 = getSettlingTime(*data1, args.baseClockHz, index1);
    std::cout << lhead << "Result: "
      << time1 << " [ms], ";
    std::cout << "Dev2";
    if (!data.speed.isNone()) {
      switch (data.d1mode) {
      case util::TransMode::TX:
        std::cout << " Tx"; break;
      case util::TransMode::RX:
        std::cout << " Rx"; break;
      case util::TransMode::LOOP:
        std::cout << " Loop"; break;
      }
      std::cout << speed << " GHz";
    }
    std::cout << ", " << data.getD1InitVoltage() << "[V] -> " << data.getD1TargetVoltage() << "[V]" << std::endl;
    std::cout.flush();
    saveMonitoringData(args, data, *data1, repeat, 1);
    time1Opt = time1;
  }

  saveResult(data, testIndex, time0, time1Opt, hw == util::PMBusType::HardWare, resultOut);

  return 0;
}

/** run all test.*/
static bool runTestAll(const Args& args, const TestConfig& conf, xsdb::Xsdb& xsdb, int count)
{
  int lastSpeed = -1;

  std::ofstream result(args.resultCSVFile);

  initResult(result);

  const auto len = conf.length();
  size_t skip = 0;
  for (auto hw : { util::PMBusType::HardWare, util::PMBusType::SoftWare }) {
    if (args.swOnly && hw == util::PMBusType::HardWare) continue;
    if (args.hwOnly && hw == util::PMBusType::SoftWare) continue;

    for (size_t i = 0; i < len; i++) {
      const auto& data = conf[i];
      for (int r = 0; r < args.repeat; r++) {
        const auto ret = runTest(args, data, xsdb, i, count, lastSpeed, r, hw, result);
        if (ret == 1) { // skip
          skip++;
          continue;
        } else if (ret != 0) {
          std::cout << "Test failed at line " << data.lineNumber << std::endl << std::endl;
          std::cout << "Please power off KC705. " << std::endl;
          std::cout << "** Due to the risk of device damage, it is imperative that you turn off the device before proceeding, and turn it back on again to run this program." << std::endl;
          return false;
        }
        lastSpeed = data.speed.speed;
      }
    }
    lastSpeed = -1;
  }
  std::cout << "All test success." << std::endl;
  if (skip != 0) {
    std::cout << skip << " tests skipped." << std::endl;
  }
  return true;
}


int main(int argc, char const* argv[])
{
  try {
    // Parse command line arguments
    Args args(argc, argv);
    if (args.help || args.error) {
      args.printHelp();
      return args.error;
    }
    args.makeDirectories();

    // Parse configuration file
    TestConfig conf(args.configFile);

    if (conf.length() == 0) {
      std::cout << "No test data in " << args.configFile << std::endl;
      return 1;
    }

    // Open XSDB
    xsdb::Xsdb xsdb(args.xsdb);
    xsdb.connect(args.url, args.port);

    // check device count
    const auto count = Device::getDeviceCount(xsdb);
    if (count == 0) {
      std::cerr << Device::NAME << " not found" << std::endl;
      return 1;
    }
    if (count > 2) {
      std::cerr << "Too many " << Device::NAME << " found." << std::endl;
      return 1;
    }

    if (!args.noWarning) {
      std::cout << "This program manipulates the power supply of a device." << std::endl;
      std::cout << "Are you sure you want to run it?" << std::endl;
      std::cout << " * You can skip this message with `-y` option." << std::endl;
      std::string x = "";
      while (x != "Y") {
        std::cout << "[Y/n] >> ";
        getline(std::cin, x);
        if (x == "n") {
          std::cout << "exit" << std::endl;
          return 0;
        }
      }
    }

    return runTestAll(args, conf, xsdb, count) ? 0 : 1;
  }
  catch (xsdb::XsdbError& e) {
    std::cerr << "[xsdb error] " << e.what() << std::endl;
    return 1;
  }
  catch (std::exception& e) {
    std::cerr << "[error] " << e.what() << std::endl;
    return 1;
  }
}

