// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "./data_checker.hpp"

void DataChecker(
  hls::stream<ap_axiu<64,0,0,0>>& data_in,
  hls::stream<ap_axiu<64,0,0,0>>& exp_in,
  hls::stream<test_app::Cnt::type>& cnt_out,
  const ap_uint<1> cancel_test
) {

#pragma HLS interface axis port=data_in
#pragma HLS interface axis port=exp_in
#pragma HLS interface axis port=cnt_out
#pragma HLS interface mode=ap_none port=cancel_test
#pragma HLS pipeline II=1 style=flp

  static ap_uint<test_app::Ack::ERROR_BIT_CNT_WIDTH> error_beat_cnt = 0;
#pragma HLS reset variable=error_beat_cnt
  static ap_uint<test_app::Ack::ERROR_BIT_CNT_WIDTH> error_bit_cnt = 0;
#pragma HLS reset variable=error_bit_cnt
  static ap_uint<test_app::Ack::DATA_BYTE_CNT_WIDTH> data_byte_cnt = 0;
#pragma HLS reset variable=data_byte_cnt
  static bool cnt_out_asserted = false;
#pragma HLS reset variable=cnt_out_asserted

  const ap_uint<64> bitmask = 0x0000000000000001;
  ap_uint<1> exp_last = 0;

  if (cancel_test) {
    if (!exp_in.empty()) exp_in.read();
    if (!data_in.empty()) data_in.read();
    if (!cnt_out_asserted) {
      cnt_out.write(test_app::Cnt(error_bit_cnt, data_byte_cnt));
      cnt_out_asserted = true;
    }
  } else if (!data_in.empty() & !exp_in.empty()) {
    data_byte_cnt += 8;
    const auto data = data_in.read();
    const auto exp  = exp_in.read();
    exp_last = exp.last;
    if (data.data != exp.data) {
      error_beat_cnt++;
      for (int i = 0; i < 64; i++) {
        if ((data.data & (bitmask << i)) != (exp.data & (bitmask << i))) {
          error_bit_cnt++;
        }
      }
    }
    // Output result
    if (exp_last) {
      cnt_out.write(test_app::Cnt(error_bit_cnt, data_byte_cnt));
    }
  }
}
