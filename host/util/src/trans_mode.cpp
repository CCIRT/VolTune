// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <trans_mode.hpp>
#include <utilfunc.hpp>

namespace util {

static inline bool matchSpeed(std::string_view x, std::string_view s)
{
  if (x == s) return true;
  const auto sl = s.length();
  return x.length() == sl + 1 && x.substr(0, sl) == s && (x[sl] == 'g' || x[sl] == 'G');
}

TransSpeed::TransSpeed(std::string_view data)
{
  if (matchSpeed(data, "0") || matchSpeed(data, "0.0") || eq_incase(data, "none") || eq_incase(data, "n000") || data == "000")
    setSpeed(0);
  else if (data == "025" || matchSpeed(data, "2.5"))
    setSpeed(25);
  else if (matchSpeed(data, "5") || matchSpeed(data, "5.0") || data == "050")
    setSpeed(50);
  else if (matchSpeed(data, "7.5") || data == "075")
    setSpeed(75);
  else if (matchSpeed(data, "10") || matchSpeed(data, "10.0") || data == "100")
    setSpeed(100);
  else {
    std::stringstream ss;
    ss << "Unknown speed: " << data;
    throw std::runtime_error(ss.str());
  }
}

void TransSpeed::setSpeed(int s)
{
  if (s == 0) {
    txBitstreamName = rxBitstreamName = loopBitstreamName = "n000";
    clockFrequency = 0;
  } else if (s == 25) {
    txBitstreamName = "t025";
    rxBitstreamName = "r025";
    loopBitstreamName = "l025";
    clockFrequency = 31250000;
  } else if (s == 50) {
    txBitstreamName = "t050";
    rxBitstreamName = "r050";
    loopBitstreamName = "l050";
    clockFrequency = 62500000;
  } else if (s == 75) {
    txBitstreamName = "t075";
    rxBitstreamName = "r075";
    loopBitstreamName = "l075";
    clockFrequency = 93750000;
  } else if (s == 100) {
    txBitstreamName = "t100";
    rxBitstreamName = "r100";
    loopBitstreamName = "l100";
    clockFrequency = 125000000;
  } else {
    std::stringstream ss;
    ss << "Unknown speed: " << s;
    throw std::runtime_error(ss.str());
  }
  speed = s;
}

}//end namespace
