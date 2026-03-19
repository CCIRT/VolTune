#ifndef TESTUTIL_UTILFUNC_HPP__
#define TESTUTIL_UTILFUNC_HPP__

#include <string_view>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <thread>

namespace util {

/**
 * @brief compare a and b without case sensitive
 * @param a
 * @param b
 */
inline bool eq_incase(std::string_view a, std::string_view b)
{
  return std::equal(a.cbegin(), a.cend(), b.cbegin(), b.cend(),
    [](char x, char y) {return std::tolower(x) == std::tolower(y);});
}


inline double convertVoltage(uint32_t binary)
{
  constexpr double ONE = 1 << 12;
  return binary / ONE;
}

inline uint32_t convertVoltage(double voltage)
{
  constexpr uint32_t ONE = 1 << 12;
  return (uint32_t)std::round(voltage * ONE);
}

inline double clockCountToMillisec(uint64_t count, double hz)
{
  return count / hz * 1000;
}

inline void sleep(uint64_t ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

};

#endif
