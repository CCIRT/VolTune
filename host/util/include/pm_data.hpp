#ifndef HOST_VOLTAGE_PM_DATA_HPP__
#define HOST_VOLTAGE_PM_DATA_HPP__

#define COMMON_NO_HLS
#include <pm_common_host.hpp>

using L16 = power_manager::L16Base<uint16_t>;
using L11 = power_manager::L11Base<uint16_t>;
using Power = power_manager::DevicePower;
using PowerSum = power_manager::DevicePowerSum;
using PowPower = power_manager::DevicePowPower;
using PowPowerSum = power_manager::DevicePowPowerSum;

#endif
