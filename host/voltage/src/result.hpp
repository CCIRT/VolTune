// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef TEST_VOLTAGE_RESULT_HPP__
#define TEST_VOLTAGE_RESULT_HPP__

#include <vector>
#include <cstdint>
#include <optional>

/**
 * @brief check monitoring voltage is stabilized and return the time[ms]
 *
 * @param voltage monitoring voltage data
 * @param timestamp monitoring time stamp
 * @param targetVoltage target voltage
 * @param checkMillisec The period of time during which the voltage is considered to have stabilized.
 * @param errorVoltage allowable limit of error
 * @param baseClockHz clock frequency[Hz]
 * @return std::optional<double> time[ms].
 */
std::optional<double> isVoltageStabilized(
  std::vector<uint32_t> voltage,
  std::vector<uint64_t> timestamp,
  double targetVoltage,
  double checkMillisec,
  double errorVoltage,
  double baseClockHz
);

#endif
