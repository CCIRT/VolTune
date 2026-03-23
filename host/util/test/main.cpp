// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <gtest/gtest.h>

#include <bitstream.hpp>
#include <utilfunc.hpp>
#include <lane.hpp>
#include <trans_mode.hpp>

#include <config.hpp>
#include <iostream>
// The next line is only there to fool the editor's completion.
#ifndef UTIL_TEST_DIRECTORY
#define UTIL_TEST_DIRECTORY "."
#endif

using namespace util;
namespace fs = std::filesystem;

const fs::path TEST_DIR(UTIL_TEST_DIRECTORY);

// Bitstream test
TEST(util_test, findFile) {
  ASSERT_EQ(findFile(TEST_DIR, "foobar", "bit"), TEST_DIR / "foobar.bit");
  ASSERT_EQ(findFile(TEST_DIR, "foobar", ".txt"), TEST_DIR / "foobar.txt");

  ASSERT_TRUE(findFile(TEST_DIR, "foobar", "hoge").empty());
  ASSERT_TRUE(findFile(TEST_DIR, "piyo", "bin").empty());
}

TEST(util_test, TransSpeed) {
  TransSpeed t("0.0");
  ASSERT_EQ(t.speed, 0);
  ASSERT_EQ(t.txBitstreamName, "n000");
  ASSERT_EQ(t.rxBitstreamName, "n000");

  t = TransSpeed("0G");
  ASSERT_EQ(t.speed, 0);
  ASSERT_EQ(t.txBitstreamName, "n000");
  ASSERT_EQ(t.rxBitstreamName, "n000");

  t = TransSpeed("None");
  ASSERT_EQ(t.speed, 0);
  ASSERT_EQ(t.txBitstreamName, "n000");
  ASSERT_EQ(t.rxBitstreamName, "n000");

  t = TransSpeed("2.5g");
  ASSERT_EQ(t.speed, 25);
  ASSERT_EQ(t.txBitstreamName, "t025");
  ASSERT_EQ(t.rxBitstreamName, "r025");

  t = TransSpeed("025");
  ASSERT_EQ(t.speed, 25);
  ASSERT_EQ(t.txBitstreamName, "t025");
  ASSERT_EQ(t.rxBitstreamName, "r025");

  t = TransSpeed("5");
  ASSERT_EQ(t.speed, 50);
  ASSERT_EQ(t.txBitstreamName, "t050");
  ASSERT_EQ(t.rxBitstreamName, "r050");

  t = TransSpeed("5.0g");
  ASSERT_EQ(t.speed, 50);
  ASSERT_EQ(t.txBitstreamName, "t050");
  ASSERT_EQ(t.rxBitstreamName, "r050");

  t = TransSpeed("7.5");
  ASSERT_EQ(t.speed, 75);
  ASSERT_EQ(t.txBitstreamName, "t075");
  ASSERT_EQ(t.rxBitstreamName, "r075");

  t = TransSpeed("7.5g");
  ASSERT_EQ(t.speed, 75);
  ASSERT_EQ(t.txBitstreamName, "t075");
  ASSERT_EQ(t.rxBitstreamName, "r075");

  t = TransSpeed("10");
  ASSERT_EQ(t.speed, 100);
  ASSERT_EQ(t.txBitstreamName, "t100");
  ASSERT_EQ(t.rxBitstreamName, "r100");
}

TEST(util_test, getBitstreamName) {
  ASSERT_EQ(getBitstreamName(PMBusType::HardWare), "hw");
  ASSERT_EQ(getBitstreamName(PMBusType::PeripheralSoftware), "ps");
  ASSERT_EQ(getBitstreamName(PMBusType::SoftWare), "sw");

  TransSpeed t("0.0");
  ASSERT_EQ(getBitstreamName(PMBusType::HardWare, t, "100", TX), "hw_n000");
  ASSERT_EQ(getBitstreamName(PMBusType::HardWare, t, "100", RX), "hw_n000");
  ASSERT_EQ(getBitstreamName(PMBusType::PeripheralSoftware, t, "100", TX), "ps_n000");
  ASSERT_EQ(getBitstreamName(PMBusType::SoftWare, t, 0, RX), "sw_n000");

  t.setSpeed(100);
  ASSERT_EQ(getBitstreamName(PMBusType::HardWare, t, "151_515", TX), "hw_t100_c151_515");
  ASSERT_EQ(getBitstreamName(PMBusType::HardWare, t, "117_188", RX), "hw_r100_c117.188");
  ASSERT_EQ(getBitstreamName(PMBusType::HardWare, t, "117_188", LOOP), "hw_l100_c117.188");
}

TEST(util_test, getLane) {
  ASSERT_EQ(getLane("MGTAVCC"), Lane::MGTAVCC);
  ASSERT_EQ(getLane("avcc"), Lane::MGTAVCC);

  ASSERT_EQ(getLane("VCCINT"), Lane::VCCINT);
  ASSERT_EQ(getLane("int"), Lane::VCCINT);

  ASSERT_EQ(getLane("VCCAUX"), Lane::VCCAUX);
  ASSERT_EQ(getLane("aux"), Lane::VCCAUX);

  ASSERT_EQ(getLane("VCC3V3"), Lane::VCC3V3);
  ASSERT_EQ(getLane("3v3"), Lane::VCC3V3);

  ASSERT_EQ(getLane("VADJ"), Lane::VADJ);
  ASSERT_EQ(getLane("adj"), Lane::VADJ);

  ASSERT_EQ(getLane("VCC1V5"), Lane::VCC1V5);
  ASSERT_EQ(getLane("1v5"), Lane::VCC1V5);

  ASSERT_EQ(getLane("VCC2V5"), Lane::VCC2V5);
  ASSERT_EQ(getLane("2v5"), Lane::VCC2V5);

  ASSERT_EQ(getLane("MGTAVTT"), Lane::MGTAVTT);
  ASSERT_EQ(getLane("avtt"), Lane::MGTAVTT);

  ASSERT_EQ(getLane("VCCAUXIO"), Lane::VCCAUX_IO);
  ASSERT_EQ(getLane("vccaux_io"), Lane::VCCAUX_IO);

  ASSERT_EQ(getLane("VCCBRAM"), Lane::VCCBRAM);
  ASSERT_EQ(getLane("bram"), Lane::VCCBRAM);

  ASSERT_EQ(getLane("MGTVCCAUX"), Lane::MGTVCCAUX);
}

