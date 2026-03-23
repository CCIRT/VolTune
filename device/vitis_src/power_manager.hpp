// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php


#ifndef SW_hls::power_manager_HPP__
#define SW_hls::power_manager_HPP__


#define COMMON_NO_HLS
#define NAMESPACE_HLS
#include "../hls/common/pm_common_host.hpp"
#undef NAMESPACE_HLS

using Power = hls::power_manager::DevicePower;
using PowerSum = hls::power_manager::DevicePowerSum;
using PowPower = hls::power_manager::DevicePowPower;
using PowPowerSum = hls::power_manager::DevicePowPowerSum;
using L16 = hls::power_manager::L16Base<uint16_t>;
using L11 = hls::power_manager::L11Base<uint16_t>;

namespace power_manager {

struct MonitoringResult {
  Power pmin, pmax;
  PowerSum psum;
  PowPowerSum ppsum;
  uint32_t pcount;
};

void init();

/** error not occured.*/
bool ok();

/** Initialize monitoring data.*/
void initPowerMonitoring(uint8_t lane);

/** get monitoring result.*/
const MonitoringResult& getResult();

/** read power. */
void updatePowerMonitoring();

/** SET_UNDER_VOLTAGE(1) command*/
void setUnderVoltage(uint8_t lane, uint16_t voltageL16);

/** SET_PGOOD_ON_VOLTAGE(2) command.*/
void setPowerGoodOn(uint8_t lane, uint16_t voltageL16);

/** SET_PGOOD_ON_VOLTAGE(3) command.*/
void setPowerGoodOff(uint8_t lane, uint16_t voltageL16);

/** SET_VOUT(4) command.*/
void setVout(uint8_t lane, uint16_t voltageL16);

/** READ_VOUT(4) command.
 * @return uint16_t(L16) value. -1: error.
*/
int32_t readVout(uint8_t lane);

}

#endif