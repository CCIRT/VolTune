#ifndef DEVICE_HLS_DATA_CHECKER_HPP__
#define DEVICE_HLS_DATA_CHECKER_HPP__

#include <_gmp_const.h>

#include <test_app_common.hpp>

void DataChecker(
  hls::stream<ap_axiu<64,0,0,0>>& data_in,
  hls::stream<ap_axiu<64,0,0,0>>& exp_in,
  hls::stream<test_app::Cnt::type>& cnt_out,
  const ap_uint<1> cancel_test
);

#endif
