// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

//
// for host program (without Vitis HLS)
//
#ifndef COMMON_FOR_HOST_HPP__
#define COMMON_FOR_HOST_HPP__
#ifdef COMMON_NO_HLS
#include <iostream>
#include <cstdint>

// dummy to compile ap_uint<N>
template<int X, bool Y> struct ap_int_base
{
  static constexpr int width = X;
  static constexpr bool isgn_flag = Y;
  operator uint16_t() const {return 0; }
  operator int16_t() const {return 0;}
  operator int32_t() const {return 0;}
  operator uint32_t() const {return 0;}
  ap_int_base<X, Y> operator()(size_t x, size_t y) const { return *this; }
};

template<int N> using ap_uint = ap_int_base<N, false>;
template<int N> using ap_int = ap_int_base<N, true>;

#endif
#endif
