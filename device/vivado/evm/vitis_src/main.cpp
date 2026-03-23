// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <pmbus.hpp>
#include <xtime_l.h>
#include <power_manager.hpp>

#define COUNTS_PER_USECOND  (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / (2U*1000000U))

void runPMBus()
{
  // PMBus 100k : sleep = 1[s] / 100k / 4 = 2.5 [us]
  // constexpr XTime SLEEP = XTime(25 * COUNTS_PER_USECOND / 10);

  // PMBus 400k : sleep = 1[s] / 400k / 4 = 0.625[us]
  constexpr XTime SLEEP = XTime(625 * COUNTS_PER_USECOND / 1000);

  // PMBus 1M   : sleep = 1[s] / 1M / 4 = 0.25[us]
  // constexpr XTime SLEEP = XTime(25 * COUNTS_PER_USECOND / 100);

  while (pmbus::isBusy()) {
    XTime t, e;
    XTime_GetTime(&t);
    e = t + SLEEP;

    // update state machine.
    pmbus::nextStep();
    // sleep (busy loop)
    do { XTime_GetTime(&t); } while (t < e);
  }
}

int main()
{
  // 1st: initialize
  pmbus::init();

  // 2nd: set slave device address
  pmbus::setAddress(104);
  // 3rd: register command
  if (!pmbus::setReadByteCmd(0x98)) { // 0x98 => get revision of pmbus
    return 1;
  }
  // 4th: run pmbus
  runPMBus();

  auto v = pmbus::getByteData(); // pmbus::readByteCmd => pmbus::getByteData
  auto e = pmbus::getError();
  // printf("0x98 command result: %x, error: %x\n\r", v, e);

  if (e) { // check error
    return 1;
  }

  // Set PAGE
  if (!pmbus::setPage(0)) {
    return 1;
  }
  runPMBus();

  if (pmbus::getError()) {
    return 1;
  }

  // get voltage
  if (!pmbus::setReadVout()){
    return 1;
  }
  runPMBus();
  if (pmbus::getError()) {
    return 1;
  }
  auto voltage = pmbus::getWordData();

  // set voltage
  if (!pmbus::setVout(voltage >> 1)){
    return 1;
  }
  runPMBus();
  if (pmbus::getError()) {
    return 1;
  }

  // power manager sample
  power_manager::init();

  const uint8_t lane = 0;
  power_manager::setVout(lane, voltage);
  power_manager::initPowerMonitoring(lane);
  power_manager::updatePowerMonitoring();

  return 0;
}
