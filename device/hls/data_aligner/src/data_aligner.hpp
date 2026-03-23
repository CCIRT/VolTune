// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef DEVICE_HLS_DATA_ALIGNER_HPP__
#define DEVICE_HLS_DATA_ALIGNER_HPP__

#include <_gmp_const.h>

#include <ap_int.h>
#include <hls_stream.h>

const ap_uint<64> COMMA = 0xFEDCBA98765432BC;

void DataAligner(
  hls::stream<ap_uint<64>>& data_in,
  hls::stream<ap_uint<8>>&  ctrl_in,
  hls::stream<ap_uint<64>>& aligned_data_out
);

#endif
