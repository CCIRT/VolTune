#include <iostream>
#include "mb_interface.h"
#include "xtmrctr.h"

#include <pmbus.hpp>
#include <power_manager.hpp>

#define COUNTS_PER_USECOND  (XPAR_MICROBLAZE_CORE_CLOCK_FREQ_HZ / (2U*1000000U))

XTmrCtr TimerCounterInst;

static int init_timer() {
	const int Status = XTmrCtr_Initialize(&TimerCounterInst, XPAR_PMBUS_AXI_TIMER_0_DEVICE_ID);

	if (Status != XST_SUCCESS) {
		printf("Timer Initialize Error\n\r");
        return XST_FAILURE;
	}

	XTmrCtr_SetOptions(&TimerCounterInst, 0, XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_Start(&TimerCounterInst, 0);
}

static u32 get_elapsed_time(const u32 basic_time) {
  const u32 current_time = XTmrCtr_GetValue(&TimerCounterInst, 0);
  // Calculate elapsed time
  if (current_time <= basic_time) {
    // Overflow detected
    return (u32(0 - 1) - basic_time) + current_time;
  } else {
    return current_time - basic_time;
  }
}

void runPMBus()
{
  // PMBus 100k : sleep = 1[s] / 100k / 4 = 2.5 [us]
  constexpr u32 SLEEP = u32(25 * COUNTS_PER_USECOND / 10);

  // PMBus 400k : sleep = 1[s] / 400k / 4 = 0.625[us]
  // constexpr u32 SLEEP = u32(625 * COUNTS_PER_USECOND / 1000);

  // PMBus 1M   : sleep = 1[s] / 1M / 4 = 0.25[us]
  // constexpr u32 SLEEP = u32(25 * COUNTS_PER_USECOND / 100);

  while (pmbus::isBusy()) {
    // Get basic time
    const u32 basic_time = XTmrCtr_GetValue(&TimerCounterInst, 0);
    // Update state machine.
    pmbus::nextStep();
    // Sleep (busy loop)
    while (get_elapsed_time(basic_time) < SLEEP);
  }
}

int main()
{
  std::cout << "Hello MicroBlaze" << std::endl;
  init_timer();

  bool monitoring = false;
  u32 monitoring_interval = 1000 * COUNTS_PER_USECOND;
  u32 monitoring_basic_time = 0;

  while (true) {
    // Read Stream
    uint32_t input = 0;
    ngetfsl(input, 0);

    // Check Stream is invalid or not.
    bool val_fsl_isinvalid = false;
    fsl_isinvalid(val_fsl_isinvalid);
    if (val_fsl_isinvalid) {
      if (monitoring) {
        // Monitoring
        if (get_elapsed_time(monitoring_basic_time) >= monitoring_interval) {
          power_manager::updatePowerMonitoring();
          monitoring_basic_time = XTmrCtr_GetValue(&TimerCounterInst, 0);
          // std::cout << "Now monitoring" << std::endl;
          // std::cout << "monitoring_interval = " << std::dec << monitoring_interval / (1000 * COUNTS_PER_USECOND) << " ms" << std::endl;
        }
      }
      continue;
    } else {
      // Parse input
      uint8_t  cmd  = (input >> 0) & 0x7;     //  3 bit
      uint8_t  lane = (input >> 3) & 0xF;     //  4 bit
      uint16_t data = (input >> 7) & 0xFFFF;  // 16 bit
      // std::cout << " input = 0x" << std::hex << input << std::endl;
      // std::cout << " cmd   = 0x" << std::hex << int(cmd)  << std::endl;
      // std::cout << " lane  = 0x" << std::hex << int(lane) << std::endl;
      // std::cout << " data  = 0x" << std::hex << int(data) << std::endl;

      static constexpr uint8_t CLEAR_STATE = 0;
      static constexpr uint8_t SET_UNDER_VOLTAGE = 1;
      static constexpr uint8_t SET_PGOOD_ON_VOLTAGE = 2;
      static constexpr uint8_t SET_PGOOD_OFF_VOLTAGE = 3;
      static constexpr uint8_t SET_VOUT = 4;
      static constexpr uint8_t READ_VOUT = 5;
      static constexpr uint8_t START_MONITORING = 6;
      static constexpr uint8_t STOP_MONITORING = 7;

      power_manager::MonitoringResult result;

      bool ok = false;                //  1 bit
      uint16_t minP_value = 0;        // 16 bit
      uint16_t maxP = 0;              // 16 bit
      uint32_t count = 0;             // 32 bit
      uint32_t sumP[2] = {0, 0};      // 48 bit
      uint32_t sumP2[2] = {0, 0};     // 64 bit

      switch (cmd) {
        case CLEAR_STATE:
          // std::cout << "CLEAR_STATE" << std::endl;
          monitoring = false;
          monitoring_interval = 1000 * COUNTS_PER_USECOND;
          power_manager::init();
          ok = power_manager::ok();
          break;
        case SET_UNDER_VOLTAGE:
          // std::cout << "SET_UNDER_VOLTAGE" << std::endl;
          if (monitoring) {
            // std::cout << "During power monitoring, ignore this command." << std::endl;
            break;
          }
          power_manager::setUnderVoltage(lane, data);
          ok = power_manager::ok();
          break;
        case SET_PGOOD_ON_VOLTAGE:
          // std::cout << "SET_PGOOD_ON_VOLTAGE" << std::endl;
          if (monitoring) {
            // std::cout << "During power monitoring, ignore this command." << std::endl;
            break;
          }
          power_manager::setPowerGoodOn(lane, data);
          ok = power_manager::ok();
          break;
        case SET_PGOOD_OFF_VOLTAGE:
          // std::cout << "SET_PGOOD_OFF_VOLTAGE" << std::endl;
          if (monitoring) {
            // std::cout << "During power monitoring, ignore this command." << std::endl;
            break;
          }
          power_manager::setPowerGoodOff(lane, data);
          ok = power_manager::ok();
          break;
        case SET_VOUT:
          // std::cout << "SET_VOUT" << std::endl;
          if (monitoring) {
            // std::cout << "During power monitoring, ignore this command." << std::endl;
            break;
          }
          power_manager::setVout(lane, data);
          ok = power_manager::ok();
          break;
        case READ_VOUT:
          // std::cout << "READ_VOUT" << std::endl;
          if (monitoring) {
            // std::cout << "During power monitoring, ignore this command." << std::endl;
            break;
          }
          minP_value = power_manager::readVout(lane);
          ok = power_manager::ok();
          break;
        case START_MONITORING:
          // std::cout << "START_MONITORING" << std::endl;
          if (monitoring) {
            // std::cout << "During power monitoring, ignore this command." << std::endl;
            break;
          }
          monitoring = true;
          if (data != 0) monitoring_interval = data * 1000 * COUNTS_PER_USECOND;
          monitoring_basic_time = XTmrCtr_GetValue(&TimerCounterInst, 0);
          power_manager::initPowerMonitoring(lane);
          ok = power_manager::ok();
          break;
        case STOP_MONITORING:
          // std::cout << "STOP_MONITORING" << std::endl;
          monitoring = false;
          result = power_manager::getResult();
          ok = power_manager::ok();
          minP_value = result.pmin;
          maxP       = result.pmax;
          count      = result.pcount;
          sumP[0]    = (result.psum  >>  0) & 0xFFFFFFFF;
          sumP[1]    = (result.psum  >> 32) & 0x0000FFFF;
          sumP2[0]   = (result.ppsum >>  0) & 0xFFFFFFFF;
          sumP2[1]   = (result.ppsum >> 32) & 0xFFFFFFFF;
          break;
        default:
          break;
      }

      // std::cout << " ok         = 0x" << std::hex << ok         << std::endl;
      // std::cout << " minP_value = 0x" << std::hex << minP_value << std::endl;
      // std::cout << " maxP       = 0x" << std::hex << maxP       << std::endl;
      // std::cout << " count      = 0x" << std::hex << count      << std::endl;
      // std::cout << " sumP[0]    = 0x" << std::hex << sumP[0]    << std::endl;
      // std::cout << " sumP[1]    = 0x" << std::hex << sumP[1]    << std::endl;
      // std::cout << " sumP2[0]   = 0x" << std::hex << sumP2[0]   << std::endl;
      // std::cout << " sumP2[1]   = 0x" << std::hex << sumP2[1]   << std::endl;

      // Pack output
      uint32_t output[6] = {0, 0, 0, 0, 0, 0};
      output[0] |= ok;
      output[0] |= (minP_value & 0x0000FFFF) <<  1;
      output[0] |= (maxP       & 0x0000FFFF) << 17;
      output[1] |= (maxP       & 0x0000FFFF) >> 15;
      output[1] |= (count      & 0xFFFFFFFF) <<  1;
      output[2] |= (count      & 0xFFFFFFFF) >> 31;
      output[2] |= (sumP[0]    & 0xFFFFFFFF) <<  1;
      output[3] |= (sumP[0]    & 0xFFFFFFFF) >> 31;
      output[3] |= (sumP[1]    & 0x0000FFFF) <<  1;
      output[3] |= (sumP2[0]   & 0xFFFFFFFF) << 17;
      output[4] |= (sumP2[0]   & 0xFFFFFFFF) >> 15;
      output[4] |= (sumP2[1]   & 0xFFFFFFFF) << 17;
      output[5] |= (sumP2[1]   & 0xFFFFFFFF) >> 15;

      // Write Stream
      for (int i = 0; i < 6; i++) {
        // std::cout << "output[" << i << "] = 0x" << std::hex << output[i] << std::endl;
        putfsl(output[i], 0);
      }
    }
  }

  return 0;
}
