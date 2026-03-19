#include "./result.hpp"
#include <utilfunc.hpp>
#include <stdexcept>

std::optional<double> isVoltageStabilized(
  std::vector<uint32_t> voltage,
  std::vector<uint64_t> timestamp,
  double targetVoltage,
  double checkMillisec,
  double errorVoltage,
  double baseClockHz
) {
  size_t len = voltage.size();

  if (len != timestamp.size()) {
    throw std::runtime_error("voltage.size() != timestamp.size()");
  }

  bool in = false;
  uint64_t inTime = 0;
  const auto lV = targetVoltage - errorVoltage;
  const auto hV = targetVoltage + errorVoltage;

  for (size_t i = 0; i < len; i++) {
    const auto v = util::convertVoltage(voltage[i]);
    if (v < lV || hV < v) {
      in = false;
      continue;
    }
    const auto t = timestamp[i];
    if (in) {
      if (checkMillisec <= util::clockCountToMillisec(t - inTime, baseClockHz)) {
        return util::clockCountToMillisec(inTime - timestamp[0], baseClockHz);
      }
    } else {
      in = true;
      inTime = t;
    }
  }
  return std::nullopt;
}
