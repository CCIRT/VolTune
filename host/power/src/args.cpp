// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "./args.hpp"
#include <string_view>
#include <sstream>
#include <iostream>
#include <charconv>
#include <parse_double.hpp>

void Args::printHelp() const
{
  if (error)
    std::cout << "ERROR! " << errorMessage.str() << std::endl;

  std::cout << exec << " <CONFIG FILE> [Options]" << std::endl;
  std::cout << std::endl
    << "  Argument:" << std::endl
    << "    CONFIG FILE: test configuration csv file." << std::endl
    << std::endl
    << "  Options:" << std::endl
    << "    -h      : show this help." << std::endl
    << "    -b DIR  : Bitstream directory path. The default path is `./bitstream`." << std::endl
    << "    -c MHz  : External clock Frequency[MHz]. The default clock is 100." << std::endl
    << "    -o FILE : Voltage csv file. The default file name is power_result.csv." << std::endl
    << "    -p PORT : hw_server port." << std::endl
    << "    -r SIZE : Number of times the same test is run repeatedly." << std::endl
    << "    -s SPEED: PMBus speed. SPEED=100k,400k,1m. The default value is 400k." << std::endl
    << "    -u URL  : hw_server url." << std::endl
    << "    -x PATH : xsdb path." << std::endl
    << "    -y      : start test without warning message." << std::endl
    << "    -w SEC  : Max test wait time[s]. Default is 0(infinit)" << std::endl
    << "    -i      : Invert Tx/Rx device." << std::endl
    << "    -sw     : Test software PowerManager only" << std::endl
    << "    -hw     : Test hardware PowerManager only" << std::endl
    << "      --log : show information level log." << std::endl
    ;
}

Args::Args(int argc, const char** argv)
  : exec(argv[0])
{
  bool arg2Fail = false;
  for (int i = 1;i < argc; i++) {
    const bool last = i + 1 == argc;
    const std::string_view arg = argv[i];
    const std::string_view arg2 = last ? "" : argv[i + 1];
    const auto f = arg2.cbegin();
    const auto e = arg2.cend();
    int inc = 1;

    if (arg == "-h" || arg == "--help") {
      help = true;
      inc = 0;
    } else if (arg == "-y") {
      noWarning = true;
      inc = 0;
    } else if (arg == "-sw") {
      swOnly = true;
      hwOnly = false;
      inc = 0;
    } else if (arg == "-hw") {
      swOnly = false;
      hwOnly = true;
      inc = 0;
    } else if (arg == "-b") {
      if (!last) {
        bitstreamDirectory = arg2;
      } else {
        arg2Fail = true;
      }
    } else if (arg == "-u") {
      if (!last) {
        url = arg2;
      } else {
        arg2Fail = true;
      }
    } else if (arg == "-p") {
      if (!last) {
        uint16_t pv;
        const auto ret = std::from_chars(f, e, pv);
        if (ret.ec != std::errc{}) {
          error = true;
          errorMessage << "parse error: " << arg << " " << arg2 << std::endl;
        }
        port = pv;
      } else {
        arg2Fail = true;
      }
    } else if (arg == "-w") {
      if (!last) {
        uint64_t v;
        const auto ret = std::from_chars(f, e, v);
        if (ret.ec != std::errc{}) {
          error = true;
          errorMessage << "parse error: " << arg << " " << arg2 << std::endl;
        }
        maxWaitTime = v;
      } else {
        arg2Fail = true;
      }
    } else if (arg == "-s") {
      if (!last) {
        if (arg2 == "100k" || arg2 == "100K") {
          pmbusSpeed = 0;
        } else if (arg2 == "400k" || arg2 == "400K") {
          pmbusSpeed = 1;
        } else if (arg2 == "1m" || arg2 == "1M") {
          pmbusSpeed = 2;
        } else {
          error = true;
          errorMessage << "Unknown PMBus speed: " << arg2 << std::endl;
        }
      } else {
        arg2Fail = true;
      }
    } else if (arg == "-x") {
      if (!last) {
        xsdb = argv[i + 1];
      } else {
        arg2Fail = true;
      }
    } else if (arg == "-c") {
      if (!last) {
        const auto ret = parse_double(arg2);
        if (!ret.ok) {
          error = true;
          errorMessage << "parse error: " << arg << " " << arg2 << std::endl;
        }
        const auto posd = arg2.find('.');

        if (posd != std::string_view::npos) {
          externalClock = arg2;
          externalClock.replace(posd, 1, "_");
        } else {
          std::stringstream ss;
          ss << arg2;
          ss << "_000";
          externalClock = ss.str();
        }
        externalClockHz = uint32_t(ret.value * 1000 * 1000);
      } else {
        arg2Fail = true;
      }
    } else if (arg == "-r") {
      if (!last) {
        int x;
        const auto ret = std::from_chars(f, e, x);
        if (ret.ec != std::errc{}) {
          error = true;
          errorMessage << "parse error: " << arg << " " << arg2 << std::endl;
        }
        if (x < 1) {
          error = true;
          errorMessage << "-r >= 1: " << arg << " " << arg2 << std::endl;
        }
        repeat = x;
      } else {
        arg2Fail = true;
      }
    } else if (arg == "--log") {
      showLog = true;
      inc = 0;
    } else if (arg == "-i") {
      invertTx = true;
      inc = 0;
    } else if (arg == "-o") {
      if (!last) {
        resultCSVFile = arg2;
      } else {
        arg2Fail = true;
      }
    } else if (arg[0] == '-') {
      error = true;
      errorMessage << "Unknown option: " << arg << std::endl;
      inc = 0;
    } else {
      if (configFile.empty() ) {
        configFile = arg;
      } else {
        error = true;
        errorMessage << "Unknown arguments:" << configFile << ", " << arg << std::endl;
      }
      inc = 0;
    }
    i += inc;
  } // end for

  // check missing value
  if (arg2Fail) {
    error = true;
    errorMessage << "Invalid option: " << argv[argc - 1] << " nead value." << std::endl;
  }

  if (configFile == "") {
    error = true;
    errorMessage << "Specify the configuration file." << std::endl;
  } else if (!std::filesystem::exists(configFile)) {
    error = true;
    errorMessage << "Config file not found: " << configFile << std::endl;
  }

  // check bitstream directory exists
  if (!std::filesystem::exists(bitstreamDirectory)) {
    error = true;
    errorMessage << "Bitstream directory `" << bitstreamDirectory << "` is not exists. " << std::endl
      << "  * This path can be changed by the option '-b <DIR>'." << std::endl;
  }
}


void Args::makeDirectories() const
{
  using namespace std::filesystem;
  const auto p = resultCSVFile.parent_path();

  if (!p.empty() && !is_directory(p))
    create_directories(p);
}
