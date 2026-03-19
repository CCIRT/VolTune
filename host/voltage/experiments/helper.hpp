#ifndef HOST_EVM_HELPER_HPP__
#define HOST_EVM_HELPER_HPP__

#include <xsdb.hpp>
#include <cstdint>
#include <chrono>
#include <thread>
#include "../src/regs.hpp"

inline void sleep(uint64_t ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// TODO:
inline uint32_t getDeviceJTAG(xsdb::Xsdb& x)
{
  const auto targets = x.targets();
  for (auto i = 0; i < targets.length(); i++) {
    if (targets[i].name == "xc7k325t") {
      return targets[i].jtag;
      break;
    }
  }
  return 0xFFFFFFFF;
}

// TODO: 
inline uint32_t getJTAG2AXIJTAG(xsdb::Xsdb& x)
{
  const auto targets = x.targets();
  for (auto i = 0; i < targets.length(); i++) {
    if (targets[i].selected) {
      auto& t = targets[i];
      for (auto j = 0; j < t.lenght(); j++) {
        auto& tj = t[j];
        if (tj.name == "Legacy Debug Hub") {
          for (auto k = 0; k < t[j].lenght(); k++) {
            auto& tjk = tj[k];
            if (tjk.name == "JTAG2AXI") {
              return t[j][k].jtag;
            }
          }
        }
      }
      return 0xFFFFFFFF;
    }
  }
  return 0xFFFFFFFF;
}


#endif

