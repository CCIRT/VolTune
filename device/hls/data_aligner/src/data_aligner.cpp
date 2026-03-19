#include "./data_aligner.hpp"

void DataAligner(
  hls::stream<ap_uint<64>>& data_in,
  hls::stream<ap_uint<8>>&  ctrl_in,
  hls::stream<ap_uint<64>>& aligned_data_out
) {

#pragma HLS interface axis port=data_in
#pragma HLS interface axis port=ctrl_in
#pragma HLS interface axis port=aligned_data_out
#pragma HLS pipeline II=1 style=flp

  static int shift = 0;
#pragma HLS reset variable=shift
  static bool aligned = false;
#pragma HLS reset variable=aligned
  static ap_uint<64> data_prev = 0;
  static ap_uint<64> ctrl_prev = 0;

  // Read data & ctrl
  const auto data = data_in.read();
  const auto ctrl = ctrl_in.read();

  // Output data when aligned is true
  if (aligned) {
    const ap_uint<64> aligned_data = (data_prev >> (shift * 8)) | (data << (64 - shift * 8));
    if (ctrl_prev == 0) {
      aligned_data_out.write(aligned_data);
    } else if (aligned_data != COMMA){
      aligned = false;
    }
  }

  // If ctrl is not zero, set aligned false temporary.
  if (ctrl != 0) {
    aligned = false;
  }

  // If data & ctrl is valid, set aligned true
  for (int i = 0; i < 8; i++) {
    if (ctrl == (1 << i)) {
      if (((data >> (i * 8)) & 0xFF) == (COMMA & 0xFF)) {
        shift = i;
        aligned = true;
        break;
      }
    }
  }

  // Save current data & ctrl
  data_prev = data;
  ctrl_prev = ctrl;
}
