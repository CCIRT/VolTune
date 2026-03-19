#include <cmath>
#include <iostream>
#include <fstream>
#include <xsdb.hpp>
#include <bitstream.hpp>
#include <kc705.hpp>
#include <utilfunc.hpp>
#include <iomanip>

#include "./regs.hpp"
#include "./args.hpp"
#include "./config.hpp"

using Device = kc705;

constexpr int BitstreamNotFound = -1;
constexpr int ClockCheckError = -2;
constexpr int SUCCESS = 0;
constexpr int JtagError = 1;
constexpr int TimeoutError = 2;
constexpr int ResultError = 3;

constexpr int WaitTimeout = -2;
constexpr int WaitJtagError = -1;


static bool cancelTest(xsdb::Xsdb& xsdb, int deviceIndex)
{
  const auto dj = Device::getDevice(xsdb, deviceIndex);
  if (!dj) return false;
  xsdb.target(*dj);
  const auto jj = Device::getJTAG(xsdb);
  if (!jj) return false;
  xsdb.target(*jj);

  Regs<xsdb::Xsdb> regs(xsdb);
  regs.disableTest();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  return true;
}

static void cancelAll(xsdb::Xsdb& xsdb, int deviceIndex)
{
  const auto dj = Device::getDevice(xsdb, deviceIndex);
  if (!dj) return;
  xsdb.target(*dj);
  const auto jj = Device::getJTAG(xsdb);
  if (!jj) return;
  xsdb.target(*jj);

  Regs<xsdb::Xsdb> regs(xsdb);
  regs.disableTest();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  regs.disablePMBus();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

struct CancelRAII
{
  xsdb::Xsdb& xsdb;
  int deviceIndex;
  bool doCancel;
  ~CancelRAII() {
    if (doCancel) {
      cancelAll(xsdb, deviceIndex);
    }
  }
};

static int initTest(
  const Args& args,
  const TestData& data,
  xsdb::Xsdb& xsdb,
  int lastTestSpeed,
  util::PMBusType busType,
  util::TransMode mode,
  int deviceIndex
) {
  const bool writeBitstream = data.speed.speed != lastTestSpeed;
  Regs<xsdb::Xsdb> regs(xsdb);
  if (writeBitstream && lastTestSpeed != -1) {
    // At the risk of the user test application forgetting to stop power monitoring, cancel it.
    // Safety cannot be guaranteed immediately after the program is started (lastTstSpeed == -1).
    cancelAll(xsdb, deviceIndex);
  }

  const auto dj = Device::getDevice(xsdb, deviceIndex);
  if (!dj) {
    std::cout << "[ERROR] Device not found." << std::endl;
    return JtagError;
  }
  if (args.showLog)
    std::cout << "[INFO] target " << (*dj) << std::endl;
  xsdb.target(*dj);


  const auto name = util::getBitstreamName(busType, data.speed, args.externalClock, mode);
  const auto bitstream = util::findBitstream(args.bitstreamDirectory, name);
  if (bitstream.empty()) {
    std::cout << "[ERROR] line " << data.lineNumber << " bitstream (" << name << "*.bit) is not found." << std::endl;
    return BitstreamNotFound;
  }
  if (!std::filesystem::exists(bitstream)) {
    std::cout << "[ERROR] line " << data.lineNumber << " bitstream (" << name << "*.bit) is not found." << std::endl;
    return BitstreamNotFound;
  }
  // update bitstream
  if (writeBitstream) {
    if (args.showLog)
      std::cout << "[INFO] write bitstream : " << bitstream.generic_string() << std::endl;
    xsdb.writeBitstream(bitstream);
  }

  const auto jj = Device::getJTAG(xsdb);
  if (!jj) {
    std::cout << "[ERROR] JTAG2AXI not found. Make sure Bitstream and design are correct. " << bitstream << std::endl;
    return JtagError;
  }
  xsdb.target(*jj);

  if (args.showLog) {
    std::cout << "[INFO] target " << (*jj) << std::endl;
    std::cout << "[INFO] init registers" << std::endl;
  }

  if (writeBitstream) {
    if (args.showLog) {
      std::cout << "[INFO] Check external clock" << std::endl;
    }
    regs.startClockCount();
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    auto c = regs.getClockCount();
    while (!c) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      c = regs.getClockCount();
    }
    uint32_t count = *c * 10;
    uint32_t clock = data.speed.clockFrequency;
    bool ok = clock - 10000 <= count && count <= clock + 10000;

    if (!ok) {
      std::cout << "[ERROR] External clock frequency check failed." << std::endl;
      std::cout << "        Check the clock setting supplied by the clock board." << std::endl;
      return ClockCheckError;
    }
  }

  // Set registers
  switch (args.pmbusSpeed) {
  case 1: regs.setI2C_400kHz(); break;
  case 2: regs.setI2C_1MHz(); break;
  default: regs.setI2C_100kHz(); break;
  }

  for (size_t i = 0; i < data.params.size(); i++) {
    if (args.showLog)
      std::cout << "[INFO] param[" << i << "] = " << data.params[i] << std::endl;
    regs.setTestParameter(data.params[i], i);
  }

  return 0;
}

/**
 * run test manager
 * @param xsdb
 */
static bool setVoltage(const Args& args, xsdb::Xsdb& xsdb, int deviceIndex, double v)
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

  if (args.showLog) {
    std::cout << "[INFO] set " << deviceIndex << " voltage:" << v << "[V]" << "(" << L16((float)v).as_uint16_t() << ")" << std::endl;
  }
  Regs<xsdb::Xsdb> regs(xsdb);

  regs.setTestVoltage(v);
  return true;
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

  Regs<xsdb::Xsdb> regs(xsdb);
  regs.enableTest();
  regs.enablePMBus();
  regs.run();
  if (args.showLog) {
    std::cout << "[INFO] run " << deviceIndex << ": JTAG ID " << *jj << std::endl;
  }
  return true;
}


/**
 * wait test manager done.
 * @param xsdb
 * @return true success
 * @return false fail
 */
static int waitTest(const Args& args, xsdb::Xsdb& xsdb, int deviceIndex, uint64_t maxWaitSec)
{
  const auto dj = Device::getDevice(xsdb, deviceIndex);
  if (!dj) {
    std::cout << "[ERROR] Fail to run test. Device not found." << std::endl;
    return WaitJtagError;
  }
  xsdb.target(*dj);
  const auto jj = Device::getJTAG(xsdb);
  if (!jj) {
    std::cout << "[ERROR] Fail to run test. JTAG2AXI not found." << std::endl;
    return WaitJtagError;
  }
  xsdb.target(*jj);

  Regs<xsdb::Xsdb> regs(xsdb);

  if (args.showLog) {
    std::cout << "[INFO] wait " << deviceIndex << ": JTAG ID " << *jj << std::endl;
  }
  const auto start = std::chrono::system_clock::now();
  while (!regs.isDone()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if (maxWaitSec != 0) {
      if (std::chrono::seconds(maxWaitSec) < std::chrono::system_clock::now() - start) {
        std::cout << "[ERROR] timeout" << std::endl;
        return WaitTimeout;
      }
    }
  }

  auto ret = regs.getResult();
  if (args.showLog) {
    std::cout << "[INFO] done: JTAG ID " << *jj << ", result " << ret << std::endl;
  }

  return ret;
}

static std::optional<Data> getData(const Args& args, xsdb::Xsdb& xsdb, int deviceIndex)
{
  const auto dj = Device::getDevice(xsdb, deviceIndex);
  if (!dj) {
    std::cout << "[ERROR] Fail to run test. Device not found." << std::endl;
    return std::nullopt;
  }
  xsdb.target(*dj);
  const auto jj = Device::getJTAG(xsdb);
  if (!jj) {
    std::cout << "[ERROR] Fail to run test. JTAG2AXI not found." << std::endl;
    return std::nullopt;
  }
  xsdb.target(*jj);

  Regs<xsdb::Xsdb> regs(xsdb);
  if (args.showLog) {
    std::cout << "[INFO] read data: JTAG ID " << *jj << std::endl;
  }

  return regs.getData();
}


static void initResult(
  std::ostream& out
) {
  out << "Line, "
    "Device1 TX/RX, Voltage[V], Device1 Data Size[bit], Device1 Error Count[bit], Device1 BER[%], Device1 Average[W], Device1 Min[W], Devie1 Max[W], Device1 Standard deviation[W],"
    "Device2 TX/RX, Voltage[V], Device2 Data Size[bit], Device2 Error Count[bit], Device2 BER[%], Device2 Average[W], Device2 Min[W], Devie2 Max[W], Device2 Standard deviation[W],"
    "Latency[ns], Latency[cycle], SW/HW"
    << std::endl;
  out.flush();
}

static void saveResult(
  const TestData& data,
  size_t testIndex,
  bool firstTx,
  double v0,
  double v1,
  const Data& d0,
  const Data& d1,
  bool hw,
  std::ostream& out
) {
  out << data.lineNumber;

  out << "," << (firstTx ? "Tx" : "Rx")
    << "," << v0
    << "," << d0.dataSize;

  if (firstTx)
    out << ",---,---";
  else  out << ", " << d0.errorBitCount << ", " << (d0.getErrorBitRate() * 100);

  out << "," << d0.averagePower
    << "," << d0.minPower
    << "," << d0.maxPower
    << "," << d0.standardDeviation
    << "," << (!firstTx ? "Tx" : "Rx")
    << "," << v1
    << "," << d1.dataSize;

  if (!firstTx)
    out << ",---,---";
  else  out << ", " << d1.errorBitCount << ", " << (d1.getErrorBitRate() * 100);

  out << "," << d1.averagePower
    << "," << d1.minPower
    << "," << d1.maxPower
    << "," << d1.standardDeviation;

  if (!firstTx) {
    out << "," << d0.getLatencyNs(data.speed) << "," << d0.latency;
  } else {
    out << "," << d1.getLatencyNs(data.speed) << "," << d1.latency;
  }
  out << "," << (hw ? "HW" : "SW") << std::endl;
  out.flush();
}

static void initLoopBackResult(
  std::ostream& out
) {
  out << "Line, Voltage,"
    "Data Size, Error Count, BER, Average[W], Min[W], Max[W], Standard deviation[W], Latency[ns], Latency[cycle], Cancel, SW/HW"
    << std::endl;
  out.flush();
}

static void saveLoopbackResult(
  const TestData& data,
  size_t testIndex,
  double v0,
  const Data& d0,
  bool hw,
  std::ostream& out
) {
  out << data.lineNumber
    << "," << v0
    << ", " << d0.dataSize
    << ", " << d0.errorBitCount
    << ", " << (d0.getErrorBitRate() * 100)
    << "," << d0.averagePower
    << "," << d0.minPower
    << "," << d0.maxPower
    << "," << d0.standardDeviation
    << "," << d0.getLatencyNs(data.speed)
    << "," << d0.latency
    << "," << (hw ? "HW" : "SW");
  out.flush();
}

static int runTest(
  const Args& args,
  const TestData& data,
  xsdb::Xsdb& xsdb,
  size_t testIndex,
  int lastSpeed,
  int repeat,
  std::ostream& resultOut,
  bool loopback,
  bool reverseTx,
  util::PMBusType hw)
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
  std::cout << lhead << "Start : Data = " << data.text << std::endl;
  std::cout.flush();

  // write bitstream, set register

  const auto firstTxMode = reverseTx ? util::RX : util::TX;
  const auto secondTxMode = reverseTx ? util::TX : util::RX;
  const auto firstMode = loopback ? util::LOOP : firstTxMode;
  const auto secondMode = loopback ? util::LOOP : secondTxMode;
  int txDevIndex = firstMode == util::RX ? 1 : 0;
  int rxDevIndex = firstMode == util::RX ? 0 : 1;

  const auto ls = lastSpeed == -1 ? -1 : -2;
  const auto ret0 = initTest(args, data, xsdb, ls, hw, firstMode, 0);
  if (ret0 < 0)  return ret0;

  if (ret0 != 0) {
    std::cout << lhead << "Faild. Device not found" << std::endl;
    return ret0;
  }

  if (!loopback) {
    const auto ret1 = initTest(args, data, xsdb, ls, hw, secondMode, 1);
    if (ret1 < 0)  return ret1;
    if (ret1 != 0) {
      std::cout << lhead << "Faild. Device not found" << std::endl;
      return ret1;
    }
  }

  // force stop PowerManager(RAII)
  CancelRAII cancelDev0{ xsdb, 0, true };
  CancelRAII cancelDev1{ xsdb, 1, !loopback };


  size_t endstep = data.step;
  size_t divstep = data.step;
  if (data.txInitVoltage == data.txLastVoltage && data.rxInitVoltage == data.rxLastVoltage) {
    endstep = 0;
  }
  if (endstep == 0) {
    divstep = 1;
  }

  for (size_t step = 0; step <= endstep; step++) {
    double txV = (data.txInitVoltage * (divstep - step) + data.txLastVoltage * step) / divstep;
    double rxV = (data.rxInitVoltage * (divstep - step) + data.rxLastVoltage * step) / divstep;
    double v0, v1;

    // set voltage
    if (loopback) {
      std::cout << lhead << " Test Voltage:" << txV << "[V]" << std::endl;
      std::cout.flush();
      v0 = v1 = txV;
    } else {
      std::cout << lhead << " Test Tx Voltage: " << txV << "[V], Rx Voltage: " << rxV << "[V]" << std::endl;
      std::cout.flush();
      v0 = firstMode == util::TX ? txV : rxV;
      v1 = firstMode == util::TX ? rxV : txV;

      if (!setVoltage(args, xsdb, rxDevIndex, rxV)) {
        std::cout << lhead << "Faild. Device not found" << std::endl;
        return JtagError;
      }
    }
    if (!setVoltage(args, xsdb, txDevIndex, txV)) {
      std::cout << lhead << "Faild. Device not found" << std::endl;
      return JtagError;
    }

    // run
    if (!loopback) {
      if (!runTest(args, xsdb, rxDevIndex)) {
        std::cout << lhead << "Faild. Device not found" << std::endl;
        return JtagError;
      }
    }
    if (!runTest(args, xsdb, txDevIndex)) {
      std::cout << lhead << "Faild. Device not found" << std::endl;
      return JtagError;
    }

    // wait
    {
      int ret = waitTest(args, xsdb, txDevIndex, args.maxWaitTime);

      if (ret != 0) {
        std::cout << lhead << "Faild";
        if (ret == WaitJtagError) std::cout << ". Device not found.";
        else if (ret == WaitTimeout) std::cout << ". Tx timeout.";
        else           std::cout << ". Error code: " << ret;
        std::cout << std::endl;

        return ret == WaitJtagError ? JtagError :
          ret == WaitTimeout ? TimeoutError : ResultError;
      }
    }
    if (!loopback) {
      int ret = waitTest(args, xsdb, rxDevIndex, 10);
      if (ret == WaitTimeout) {
        Regs<xsdb::Xsdb> regs(xsdb);
        if (cancelTest(xsdb, rxDevIndex)) {
          std::this_thread::sleep_for(std::chrono::milliseconds(10000));
          const auto result = regs.getResult();
          if (!regs.isDone() || result != 0xC) {
            std::cout << "[ERROR] Rx timeout: result = " << result << std::endl;
            return TimeoutError;
          }
        } else {
          std::cout << "[ERROR] Rx timeout: cancel failed. " << std::endl;
          return TimeoutError;
        }
      } else  if (ret != 0) {
        std::cout << lhead << "Faild";
        if (ret == WaitJtagError) std::cout << ". Device not found.";
        else           std::cout << ". Error code: " << ret;
        std::cout << std::endl;
        return ret == WaitJtagError ? JtagError : ResultError;
      }
    }

    // show result
    if (!loopback) {
      auto d0 = getData(args, xsdb, 0);
      if (!d0) {
        std::cout << lhead << "Faild. Device not found" << std::endl;
        return JtagError;
      }
      auto d1 = getData(args, xsdb, 1);
      if (!d1) {
        std::cout << lhead << "Faild. Device not found" << std::endl;
        return JtagError;
      }

      std::cout << lhead << "  Result Device1: "
        << (firstMode == util::TX ? "TX" : "RX")
        << ", BER :";
      if (firstMode == util::TX) {
        std::cout << " -- [%] (--/-- [bit/bit]), Latency: -- [ns] (--[cycle]), ";
      } else {
        std::cout << std::fixed << std::setprecision(2) << (d0->getErrorBitRate() * 100) << "[%] (" << d0->errorBitCount << "/" << d0->dataSize << " [bit/bit]), ";
        std::cout << "Latency: " << (d0->getLatencyNs(data.speed)) << "[ns](" << (d0->latency) << "[cycle]), ";
      }
      std::cout << "Power: average " << std::setprecision(4) << d0->averagePower << "[W],"
        " min " << std::setprecision(4) << d0->minPower << "[W],"
        " max " << std::setprecision(4) << d0->maxPower << "[W],"
        " standard deviation " << std::setprecision(4) << d0->standardDeviation << "[W]"
        << std::endl;

      std::cout << lhead << "  Result Device2: "
        << (secondMode == util::TX ? "TX" : "RX")
        << ", BER :";
      if (firstMode != util::TX) {
        std::cout << " -- [%] (--/-- [bit/bit]), Latency: -- [ns] (--[cycle]), ";
      } else {
        std::cout << std::fixed << std::setprecision(2) << (d1->getErrorBitRate() * 100) << "[%] (" << d1->errorBitCount << "/" << d1->dataSize << " [bit/bit]), ";
        std::cout << "Latency: " << (d1->getLatencyNs(data.speed)) << "[ns] (" << d1->latency << "[cycle]), ";
      }
      std::cout << "Power: average " << std::setprecision(4) << d1->averagePower << "[W],"
        " min " << std::setprecision(4) << d1->minPower << "[W],"
        " max " << std::setprecision(4) << d1->maxPower << "[W],"
        " standard deviation " << std::setprecision(4) << d1->standardDeviation << "[W]"
        << std::endl;

      std::cout.flush();

      saveResult(data, testIndex, firstMode == util::TX, v0, v1, *d0, *d1, hw == util::PMBusType::HardWare, resultOut);
    } else {
      auto d0 = getData(args, xsdb, 0);
      if (!d0) {
        std::cout << lhead << "Faild. Device not found" << std::endl;
        return JtagError;
      }

      std::cout << lhead << "Result Device: "
        << "Loopback"
        << ", BER :" << std::fixed << std::setprecision(2) << (d0->getErrorBitRate() * 100) << "[%],"
        "Power: average " << std::setprecision(4) << d0->averagePower << "[V],"
        " min " << std::setprecision(4) << d0->minPower << "[V],"
        " max " << std::setprecision(4) << d0->maxPower << "[V],"
        " dispersion " << std::setprecision(4) << d0->dispersionPower << "[V*V]"
        << std::endl;
      std::cout.flush();
      saveLoopbackResult(data, testIndex, v0, *d0, hw == util::PMBusType::HardWare, resultOut);
    }
  }// end for (step)
  cancelDev0.doCancel = false;
  cancelDev1.doCancel = false;
  return SUCCESS;
}

static void backupFile(std::filesystem::path p)
{
  if (!std::filesystem::exists(p)) return;

  const auto parent = p.parent_path();
  const auto ext = p.extension();
  const auto originName = p.stem();

  for (size_t i = 1;;i++) {
    auto p2 = parent / originName;
    p2 += "_bk(";
    p2 += std::to_string(i);
    p2 += ")";
    p2 += ext;
    if (!std::filesystem::exists(p2)) {
      std::filesystem::rename(p, p2);
      return;
    }
  }
}

static bool runTestAll(const Args& args, const TestConfig& conf, xsdb::Xsdb& xsdb, bool loopback)
{
  int lastSpeed = -1;

  backupFile(args.resultCSVFile);

  std::ofstream result(args.resultCSVFile);

  if (loopback)
    initLoopBackResult(result);
  else
    initResult(result);


  const auto len = conf.length();
  for (auto hw : { util::PMBusType::HardWare, util::PMBusType::SoftWare }) {
    if (args.swOnly && hw == util::PMBusType::HardWare) continue;
    if (args.hwOnly && hw == util::PMBusType::SoftWare) continue;

    for (size_t i = 0; i < len; i++) {
      const auto& data = conf[i];
      for (int r = 0; r < args.repeat; r++) {
        const auto ret = runTest(args, data, xsdb, i, lastSpeed, r, result, loopback, args.invertTx, hw);
        if (ret == BitstreamNotFound)
          continue;
        else if (ret == ClockCheckError) {
          std::cout << std::endl;
          std::cout << "[ERROR] Test failed at line " << data.lineNumber << std::endl << std::endl;
          return false;
        } else if (ret != SUCCESS) {
          std::cout << std::endl;
          std::cout << "[ERROR] Test failed at line " << data.lineNumber << std::endl << std::endl;
          std::cout << "        Please power off KC705. " << std::endl << std::endl;
          std::cout << "** Due to the risk of device damage, it is imperative that you turn off the device before proceeding, and turn it back on again to run this program." << std::endl;
          return false;
        }
        lastSpeed = data.speed.speed;
      }
    }
    lastSpeed = -1;
  }
  std::cout << "All test success." << std::endl;
  return true;
}


int main(int argc, char const* argv[])
{
  try {
    Args args(argc, argv);
    if (args.help || args.error) {
      args.printHelp();
      return args.error;
    }

    TestConfig conf(args.configFile);

    if (conf.length() == 0) {
      std::cout << "[ERROR] No test data in " << args.configFile << std::endl;
      return 1;
    }

    // Open XSDB
    xsdb::Xsdb xsdb(args.xsdb);
    xsdb.connect(args.url, args.port);
    xsdb.setTimeout(30000);

    // check device count
    const auto count = Device::getDeviceCount(xsdb);
    bool loopback = count == 1;
    if (count == 0) {
      std::cout << "[ERROR] " << Device::NAME << " not found" << std::endl;
      return 1;
    } else if (count > 2) {
      std::cout << "[ERROR] Too many " << Device::NAME << " found." << std::endl;
      return 1;
    }

    if (!args.noWarning) {
      std::cout << std::endl;
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

    args.makeDirectories();

    return runTestAll(args, conf, xsdb, loopback) ? 0 : 1;
  }
  catch (xsdb::XsdbError& e) {
    std::cerr << "[ERROR] XSDB error: " << e.what() << std::endl;
    return 1;
  }
  catch (std::exception& e) {
    std::cerr << "[ERROR] " << e.what() << std::endl;
    return 1;
  }

}
