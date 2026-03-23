// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef DEVICE_HLS_COMMON_LED_COMMON_HPP__
#define DEVICE_HLS_COMMON_LED_COMMON_HPP__

#include <ostream>
#include <cstdint>
#include <cstddef>
#include <ap_int.h>
#include <hls_stream.h>


namespace led {

struct LED {
  using type = ap_uint<3>;
  using stream = hls::stream<type>;

  type value;
  inline LED(bool run, bool ok):
    value(run ? 1 : ok ? 2 : 4) {
#pragma HLS inline
  }

  inline LED(const type& v): value(v) {
#pragma HLS inline
  }

  inline LED(): value(0) {
#pragma HLS inline
  }

  inline operator type() const {
#pragma HLS inline
    return value;
  }

  inline bool ok() const {
#pragma HLS inline
    return value[1] == 1;
  }

  inline bool fail() const {
#pragma HLS inline
    return value[2] == 1;
  }

  inline bool run() const {
#pragma HLS inline
    return value[0] == 1;
  }
};

}

inline std::ostream& operator<<(std::ostream& o, const led::LED& l)
{
  o << "run: " << (l.run()? "true": "false") << ", ok: " << (l.ok()? "true": "false") <<", fail: " << (l.fail()? "true": "false");
  return o;
}

#endif
