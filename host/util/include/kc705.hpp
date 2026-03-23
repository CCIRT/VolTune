// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef HOST_UTIL_KC705_HPP__
#define HOST_UTIL_KC705_HPP__

#include <xsdb.hpp>
#include <optional>

struct kc705 {
  static constexpr const char* NAME = "KC705";

  static constexpr const char* devname = "xc7k325t";
  /**
   * get KC705 jtag number.
   *
   * @param x xsdb
   * @param index return the `index`th device to be found.
   * @return std::optional<uint32_t> jtag number
   */
  static inline std::optional<uint32_t> getDevice(xsdb::Xsdb& x, int index) {
    const auto targets = x.targets();
    int idx = 0;
    for (size_t i = 0; i < targets.length(); i++) {
      if (targets[i].name == devname) {
        if (index == idx) {
          return targets[i].jtag;
        }
        idx++;
      }
    }
    return std::nullopt;
  }

  /**
   * get KC705 divices
   *
   * @param x xsdb
   * @return size_t
   */
  static inline size_t getDeviceCount(xsdb::Xsdb& x) {
    const auto targets = x.targets();
    size_t s = 0;
    for (size_t i = 0; i < targets.length(); i++) {
      if (targets[i].name == devname) {
        s++;
      }
    }
    return s;
  }

  /**
   * Get `Legacy Debug Hub/JTAG2AXI` JTAG number of SELECTED device.
   *
   * User MUST call this method after call `target` XSDB command.
   *
   * @param x xsdb
   * @return std::optional<uint32_t>
   */
  static inline std::optional<uint32_t> getJTAG(xsdb::Xsdb& x)
  {
    const auto targets = x.targets();
    for (size_t i = 0; i < targets.length(); i++) {
      auto& t = targets[i];
      if (!t.selected) continue;

      for (size_t j = 0; j < t.lenght(); j++) {
        auto& tj = t[j];
        if (tj.name != "Legacy Debug Hub") continue;

        for (size_t k = 0; k < tj.lenght(); k++) {
          auto& tjk = tj[k];
          if (tjk.name == "JTAG2AXI")
            return tjk.jtag;
        }
      }
      return std::nullopt;
    }
    return std::nullopt;
  }

};

#endif
