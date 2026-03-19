#include "./comma_injector.hpp"

void CommaInjector(
  hls::stream<ap_axiu<64,0,0,0>>& data_in,
  hls::stream<ap_uint<64>>& data_out,
  hls::stream<ap_uint<8>>&  ctrl_out
) {

#pragma HLS interface axis port=data_in
#pragma HLS interface axis port=data_out
#pragma HLS interface axis port=ctrl_out
#pragma HLS pipeline II=1 style=flp

  static int cnt = 0;
#pragma HLS reset variable=cnt
  static ap_axiu<64,0,0,0> data_prev;
  ap_axiu<64,0,0,0> data;

  const bool flush = ((cnt == COMMA_INTERVAL) | (cnt != 0 & data_prev.last));

  // Read data
  if (!flush) {
    data = data_in.read();
  }

  // When first data, output comma.
  if (cnt == 0) {
    data_out.write(COMMA);
    ctrl_out.write(0x1);
  } else {
    data_out.write(data_prev.data);
    ctrl_out.write(0x0);
  }

  // Update cnt val
  if (flush) {
    cnt = 0;
  } else {
    cnt++;
  }

  // Save current data
  data_prev = data;
}
