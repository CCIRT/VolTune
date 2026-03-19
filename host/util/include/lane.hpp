#ifndef TESTUTIL_LANE_HPP__
#define TESTUTIL_LANE_HPP__
#include <string_view>
#include <string>
#include <algorithm>
#include "./utilfunc.hpp"

namespace util {

enum class Lane: uint8_t
{
  // U55, address 52
  VCCINT = 0,
  VCCAUX = 1,
  VCC3V3 = 2,
  VADJ = 3,
  // U56, address 53
  VCC1V5 = 4,
  VCC2V5 = 5,
  MGTAVCC = 6,
  MGTAVTT = 7,
  // U89, address 54
  VCCAUX_IO = 8,
  VCCBRAM = 9,
  MGTVCCAUX = 10,

  Unknown = 255
};

/**
 * Get the Lane enum value from string
 *
 * @param name lane name
 * @return Lane
 */
inline Lane getLane(std::string_view name)
{
  if (eq_incase(name, "vccint") || eq_incase(name, "int"))
    return Lane::VCCINT;
  else if (eq_incase(name, "vccaux") || eq_incase(name, "aux"))
    return Lane::VCCAUX;
  else if (eq_incase(name, "vcc3v3") || eq_incase(name, "3v3"))
    return Lane::VCC3V3;
  else if (eq_incase(name, "vadj") || eq_incase(name, "adj"))
    return Lane::VADJ;
  else if (eq_incase(name, "vcc1V5") || eq_incase(name, "1V5"))
    return Lane::VCC1V5;
  else if (eq_incase(name, "vcc2V5") || eq_incase(name, "2V5"))
    return Lane::VCC2V5;
  else if (eq_incase(name, "mgtavcc") || eq_incase(name, "avcc"))
    return Lane::MGTAVCC;
  else if (eq_incase(name, "mgtavtt") || eq_incase(name, "avtt"))
    return Lane::MGTAVTT;
  else if (eq_incase(name, "vccaux_io") || eq_incase(name, "vccauxio") || eq_incase(name, "aux_io") || eq_incase(name, "auxio"))
    return Lane::VCCAUX_IO;
  else if (eq_incase(name, "vccbram") || eq_incase(name, "bram"))
    return Lane::VCCBRAM;
  else if (eq_incase(name, "mgtvccaux"))
    return Lane::MGTVCCAUX;
  else
    return Lane::Unknown;
}

}// end namespace

#endif
