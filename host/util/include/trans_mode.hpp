#ifndef TESTUTIL_TRANS_SPEED_HPP__
#define TESTUTIL_TRANS_SPEED_HPP__

#include <string>
#include <string_view>
#include <sstream>

namespace util {

enum class PMBusType
{
  /** RTL implements */
  HardWare,
  /** operate GPIO from C */
  SoftWare,
  /**  hardware + c */
  PeripheralSoftware
};

inline const char* getBitstreamName(PMBusType t)
{
  switch(t) {
    case PMBusType::HardWare: return "hw";
    case PMBusType::SoftWare: return "sw";
    case PMBusType::PeripheralSoftware: return "ps";
  }
  return "";
}


enum TransMode
{
  TX, RX, LOOP
};

struct TransSpeed
{
  int speed; // 0, 25, 50, 75, 100
  uint32_t clockFrequency;
  std::string txBitstreamName; // n000, t025, t050, t075, t100
  std::string rxBitstreamName; // n000, r025, r050, r075, r100
  std::string loopBitstreamName;// n000, l025, l050, l075, l100

  TransSpeed() = default;
  TransSpeed(std::string_view data);
  void setSpeed(int s);
  bool isNone() const { return speed == 0;}
  const std::string& getBitstreamName(TransMode mode) const {
    switch(mode) {
      case TX: return txBitstreamName;
      case RX: return rxBitstreamName;
      case LOOP: return loopBitstreamName;
    }
    throw "Fatal Error! getBitstreamName switch is not implemented";
  }
};

}

#endif
