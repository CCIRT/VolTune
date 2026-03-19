#ifndef VOLTAGE_ARGS_HPP__
#define VOLTAGE_ARGS_HPP__

#include <string>
#include <sstream>
#include <filesystem>

struct Args {

  Args(int argc, char const **argv);
  const char* exec; // argv[0]

  bool showLog = false;

  /**
   * Test configuration file.
   */
  std::filesystem::path configFile;

  /**
   * Start test without warning message. -y
   */
  bool noWarning = false;

  /**
   * allowable error margin. -e NUMBER
   *
   * The default value is 0.04 (4%)
   */
  double errorMargin = 0.04;

  /**
   * FPGA base clock. -c NUMBER
   */
  double baseClockHz = 100 * 1000 * 1000;

  /**
   * The time from setting the initial voltage to setting the target voltage.
   *
   * -w NUMBER
   *
   * The default value is 0.5[s]
   */
  double testStartWatingTime = 0.5;

  /**
   * Bitstream directory. -b DIRECTORY
   *
   * The default value is bitstream
   */
  std::filesystem::path bitstreamDirectory = "bitstream";

  /**
   * Output csv file path. -o FILE
   *
   * The default value is "v_result.csv"
   */
  std::filesystem::path resultCSVFile = "v_result.csv";

  /**
   * read voltage data size. -n SIZE
   * The default value is 100.
   */
  int readDataSize = 100;

  /**
   * Output monitoring raw voltage data direcotry path. -O DIR
   *
   * The default value is "resultCSVFile/../V"
   *
   */
  std::filesystem::path monitoringDataDirectory = "V";

  /**
   * hw_server url. -u URL
   * The default value is ""(empty string)
   */
  std::string url = "";
  /**
   * hw_server port. -p NUM
   * The default value is 0 (unspecified)
   */
  uint16_t port = 0;
  /**
   * PMBus speed. -s <SPEED>
   *  - 0: 100k
   *  - 1: 400k (default)
   *  - 2: 1m
   */
  int pmbusSpeed = 1;

  /**
   * @brief Repeat test.
   * -r SIZE
   */
  int repeat = 1;

  /**
   * Transceiver clock[MHz].
   * -c Number
   * default is 125.00 mhz
   */
  std::string externalClock = "125_000";
  uint32_t externalClockHz = 125000000;
  /**
   * Software PowerManager Only. -sw
   */
  bool swOnly = false;
  /**
   * Hardware PowerManager Only. -hw
   */
  bool hwOnly = false;

  bool error = false;
  bool help = false;
  std::stringstream errorMessage;
  /**
   * xsdb path. -x <PATH>
   */
  const char* xsdb = nullptr;
  /**
   * show help & error message
   */
  void printHelp() const;

  /**
   * create output directories.
   */
  void makeDirectories() const;
};


#endif
