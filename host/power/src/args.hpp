// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef POWER_ARGS_HPP__
#define POWER_ARGS_HPP__

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
   * Bitstream directory. -b DIRECTORY
   *
   * The default value is bitstream
   */
  std::filesystem::path bitstreamDirectory = "bitstream";

  /**
   * Output csv file path. -o FILE
   *
   * The default value is "power_result.csv"
   */
  std::filesystem::path resultCSVFile = "power_result.csv";


  /**
   * xsdb path. -x <PATH>
   */
  const char* xsdb = nullptr;

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
   * default is 125.00
   */
  std::string externalClock = "125_000";
  uint32_t externalClockHz = 125000000;
  /**
   * max test wait time[s]
   * -w sec
   *
   * default is 0 (infinit)
   */
  uint64_t maxWaitTime = 0;

  bool invertTx = false;
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
   * show help & error message
   */
  void printHelp() const;
  /**
   * create output directories.
   */
  void makeDirectories() const;
};


#endif
