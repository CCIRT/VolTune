// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef DEVICE_HLS_COMMA_INJECTOR_HPP__
#define DEVICE_HLS_COMMA_INJECTOR_HPP__

#include <_gmp_const.h>

#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>

const ap_uint<64> COMMA = 0xFEDCBA98765432BC;
constexpr int COMMA_INTERVAL = 15;

void CommaInjector(
  hls::stream<ap_axiu<64,0,0,0>>& data_in,
  hls::stream<ap_uint<64>>& data_out,
  hls::stream<ap_uint<8>>&  ctrl_out
);

#endif
