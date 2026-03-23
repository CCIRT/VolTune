// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <gtest/gtest.h>
#include <fstream>
#include "args.hpp"
#include "config.hpp"
#include "test_config.hpp"

namespace fs = std::filesystem;

TEST(voltage, args) {
  const char *argv[] = {
    "piyo.exe",
    "-y",
    TEST_SRC_DIR "/CMakeLists.txt",
    "-e", "0.1",
    "-c", "128",
    "-f", "300",
    "-w", "0.2",
    "-b", "hoge",
    "-o", "piyo/hoge.csv",
    "-u", "hogehoge",
    "-p", "120",
    "hoge/conf.csv"
  };
  fs::create_directory("hoge"); // bitstream directory,and hoge/conf.csv
  {
    std::ofstream ofs("hoge/conf.csv");
    ofs << "dummy" << std::endl;
  }

  constexpr int SIZE = sizeof(argv) / sizeof(const char*);
  Args x(SIZE, argv);
  std::cout << x.errorMessage.str() << std::endl;
  ASSERT_FALSE(x.error);
  ASSERT_FALSE(x.help);

  ASSERT_TRUE(x.noWarning);
  ASSERT_DOUBLE_EQ(x.errorMargin, 0.1);
  ASSERT_DOUBLE_EQ(x.baseClockHz, 300*1000*1000);
  ASSERT_EQ(x.externalClock, "128_000");
  ASSERT_DOUBLE_EQ(x.testStartWatingTime, 0.2);
  ASSERT_EQ(x.bitstreamDirectory, fs::path("hoge"));
  ASSERT_EQ(x.resultCSVFile, fs::path("piyo")/"hoge.csv");
  ASSERT_EQ(x.monitoringDataDirectory, fs::path("piyo")/"V");
  ASSERT_EQ(x.url, "hogehoge");
  ASSERT_EQ(x.port, 120);
}

TEST(voltage, config) {
  using namespace util;
  TestConfig c(TEST_SRC_DIR "/test/test.csv");
  ASSERT_EQ(c.length(), 7);
  ASSERT_EQ(c[0].lineNumber, 1);
  ASSERT_EQ(c[1].lineNumber, 2);
  ASSERT_EQ(c[2].lineNumber, 7);

  ASSERT_EQ(c[3].lineNumber, 5);

  ASSERT_EQ(c[4].lineNumber, 9);

  ASSERT_EQ(c[5].lineNumber, 11);

  ASSERT_EQ(c[6].lineNumber, 3);


  ASSERT_EQ(c[0].lane, Lane::MGTAVCC);
  ASSERT_EQ(c[1].lane, Lane::MGTAVCC);
  ASSERT_EQ(c[2].lane, Lane::VCCAUX);
  ASSERT_EQ(c[3].lane, Lane::VCC3V3);
  ASSERT_EQ(c[4].lane, Lane::MGTAVTT);
  ASSERT_EQ(c[5].lane, Lane::MGTAVTT);
  ASSERT_EQ(c[6].lane, Lane::MGTAVCC);

  ASSERT_EQ(c[0].speed.speed, 0);
  ASSERT_EQ(c[1].speed.speed, 0);
  ASSERT_EQ(c[2].speed.speed, 0);

  ASSERT_EQ(c[3].speed.speed, 25);

  ASSERT_EQ(c[4].speed.speed, 50);

  ASSERT_EQ(c[5].speed.speed, 75);

  ASSERT_EQ(c[6].speed.speed, 100);

  ASSERT_DOUBLE_EQ(c[0].getD0InitVoltage(), 1.0);
  ASSERT_DOUBLE_EQ(c[0].getD0TargetVoltage(), 0.2);
  ASSERT_DOUBLE_EQ(c[0].getD1InitVoltage(), 1.0);
  ASSERT_DOUBLE_EQ(c[0].getD1TargetVoltage(), 0.2);

  ASSERT_DOUBLE_EQ(c[1].getD0InitVoltage(), 0.3);
  ASSERT_DOUBLE_EQ(c[1].getD0TargetVoltage(), 0.5);
  ASSERT_DOUBLE_EQ(c[1].getD1InitVoltage(), 0.3);
  ASSERT_DOUBLE_EQ(c[1].getD1TargetVoltage(), 0.5);


  ASSERT_DOUBLE_EQ(c[2].getD0InitVoltage(), 3.3);
  ASSERT_DOUBLE_EQ(c[2].getD0TargetVoltage(), 3.1);
  ASSERT_DOUBLE_EQ(c[2].getD1InitVoltage(), 3.2);
  ASSERT_DOUBLE_EQ(c[2].getD1TargetVoltage(), 2.9);

}
