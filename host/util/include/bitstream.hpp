#ifndef TESTUTIL_BITSTREAM_HPP__
#define TESTUTIL_BITSTREAM_HPP__

#include <filesystem>
#include <sstream>
#include <string_view>
#include <cmath>
#include <iomanip>
#include "./trans_mode.hpp"
namespace util {

std::filesystem::path findFile(
  const std::filesystem::path& directory,
  std::string_view startName,
  std::string_view extName
);

inline std::filesystem::path findBitstream(
  const std::filesystem::path& directory,
  std::string_view startName)
{
  return findFile(directory, startName, ".bit");
}

inline std::string getBitstreamName(
  PMBusType busType,
  const TransSpeed& speed,
  std::string_view baseClock,
  TransMode mode
) {
  std::stringstream ss;
  ss << getBitstreamName(busType);
  ss << "_" << speed.getBitstreamName(mode);
  if (!speed.isNone()) {
    ss << "_c" << baseClock;
  }
  return ss.str();
}


};

#endif
