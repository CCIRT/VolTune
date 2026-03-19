#include <pmbus.hpp>
#include "./power_manager.hpp"

namespace power_manager {

uint8_t gaddr;
uint8_t gpage;

MonitoringResult result;
uint8_t paddr;
uint8_t ppage;

bool error;

void init() {
  gaddr = 0xFF;
  gpage = 0xFF;
  paddr = 0xFF;
  ppage = 0xFF;
  error = false;//Note: I don't really want to erase this flag if a reset happens.
}

static bool updateLane(uint8_t addr, uint8_t page)
{
  if (addr != gaddr || page != ppage) {
    pmbus::setAddress(addr);
    pmbus::setPage(page);
    ::runPMBus();
    if (pmbus::getError()) {
      error = true;
      gaddr = 0xFF;
      gpage = 0xFF;
      return false;
    }
    gaddr = addr;
    gpage = page;
  }
  return true;
}
static inline bool updateLane(uint8_t lane)
{
  return updateLane(hls::power_manager::getLaneAddr(lane), hls::power_manager::getLanePage(lane));
}

static bool run()
{
  ::runPMBus();
  if (pmbus::getError()) {
    error = true;
    return false;
  }
  return true;
}


bool ok() { return !error; }

void initPowerMonitoring(uint8_t lane)
{
  result.pmin.clear();
  result.pmax.clear();
  result.psum.clear();
  result.ppsum.clear();
  result.pcount = 0;
  paddr = hls::power_manager::getLaneAddr(lane);
  ppage = hls::power_manager::getLanePage(lane);
}

const MonitoringResult& getResult() { return result; }

void updatePowerMonitoring()
{
  if (error) return;
  if (pmbus::isBusy()) return;
  if (!updateLane(paddr, ppage)) return;

  pmbus::setReadPout();
  if (!run()) return;

  Power p(L11(pmbus::getWordData()));
  if (result.pcount == 0) {
    result.pmin = result.pmax = p;
  } else {
    result.pmin.setMin(p);
    result.pmax.setMax(p);
  }

  result.psum += p;
  result.ppsum += p.pow();
  result.pcount++;
}

void setUnderVoltage(uint8_t lane, uint16_t voltageL16)
{
  if (error) return;
  if (pmbus::isBusy()) return;
  if (!updateLane(lane)) return;

  pmbus::setVoutUVWarnLimit(voltageL16);
  if (!run()) return;

  pmbus::setVoutUVFaultLimit(voltageL16);
  if (!run()) return;
}

void setPowerGoodOn(uint8_t lane, uint16_t voltageL16)
{
  if (error) return;
  if (pmbus::isBusy()) return;
  if (!updateLane(lane)) return;

  pmbus::setPowerGoodOn(voltageL16);
  if (!run()) return;
}

void setPowerGoodOff(uint8_t lane, uint16_t voltageL16)
{
  if (error) return;
  if (pmbus::isBusy()) return;
  if (!updateLane(lane)) return;

  pmbus::setPowerGoodOff(voltageL16);
  if (!run()) return;
}

void setVout(uint8_t lane, uint16_t voltageL16)
{
  if (error) return;
  if (pmbus::isBusy()) return;
  if (!updateLane(lane)) return;

  pmbus::setVout(voltageL16);
  if (!run()) return;
}

int32_t readVout(uint8_t lane)
{
  if (error) return -1;
  if (pmbus::isBusy()) return -1;
  if (!updateLane(lane)) return -1;
  pmbus::setReadVout();
  if (!run()) return -1;
  return int32_t(pmbus::getWordData());
}



}
