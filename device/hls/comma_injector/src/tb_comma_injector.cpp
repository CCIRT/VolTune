// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "./comma_injector.hpp"

template <typename T, typename U>
static inline T aligner(const T val_be_aligned, const U val_align_to) {
  return ((val_be_aligned + (val_align_to - 1)) / val_align_to) * val_align_to;
}

static bool test_fixture(const ap_uint<64> test_size) {

  // Generate Test Data
  std::vector<ap_uint<64>> test_data;
  std::vector<ap_uint<64>> exp_data;
  std::vector<ap_uint<8>>  exp_ctrl;
  const ap_uint<64> test_beat_num = aligner(test_size, 8) / 8;
  for (ap_uint<64> i = 0; i < test_beat_num; i++) {
    if ((i % COMMA_INTERVAL) == 0) {
      exp_data.push_back(COMMA);
      exp_ctrl.push_back(0x1);
    }
    test_data.push_back(i);
    exp_data.push_back(i);
    exp_ctrl.push_back(0x0);
  }

  // Pack Stream
  hls::stream<ap_axiu<64,0,0,0>> data_in;
  hls::stream<ap_uint<64>> data_out;
  hls::stream<ap_uint<8>>  ctrl_out;
  for (ap_uint<64> i = 0; i < test_data.size(); i++) {
    ap_axiu<64,0,0,0> data;
    data.data = test_data[i];
    data.last = (i == (test_data.size() - 1));
    data_in.write(data);
  }

  // Test Module
  for (ap_uint<64> i = 0; i < exp_data.size(); i++) {
    CommaInjector(data_in, data_out, ctrl_out);
  }

  // Unpack Stream
  std::vector<ap_uint<64>> output_data;
  std::vector<ap_uint<8>>  output_ctrl;
  while (!data_out.empty()) {
    output_data.push_back(data_out.read());
  }
  while (!ctrl_out.empty()) {
    output_ctrl.push_back(ctrl_out.read());
  }

  // Check Data
  if (output_data.size() != exp_data.size()) {
    std::cout << std::dec << "Output data size does not match" << std::endl;
    std::cout << std::dec << "output_data.size() = " << output_data.size() << std::endl;
    std::cout << std::dec << "exp_data.size()    = " << exp_data.size() << std::endl;
    return false;
  } else {
    for (ap_uint<64> i = 0; i < output_data.size(); i++) {
      if (output_data[i] != exp_data[i]) {
        std::cout << std::hex << "Output data does not match" << std::endl;
        std::cout << std::hex << "output_data[" << i << "] = " << output_data[i] << std::endl;
        std::cout << std::hex << "exp_data[" << i << "]    = " << exp_data[i] << std::endl;
        return false;
      }
    }
  }

  // Check Ctrl
  if (output_ctrl.size() != exp_ctrl.size()) {
    std::cout << std::dec << "Output ctrl size does not match" << std::endl;
    std::cout << std::dec << "output_ctrl.size() = " << output_ctrl.size() << std::endl;
    std::cout << std::dec << "exp_ctrl.size()    = " << exp_ctrl.size() << std::endl;
    return false;
  } else {
    for (ap_uint<64> i = 0; i < output_ctrl.size(); i++) {
      if (output_ctrl[i] != exp_ctrl[i]) {
        std::cout << std::hex << "Output ctrl does not match" << std::endl;
        std::cout << std::hex << "output_ctrl[" << i << "] = " << output_ctrl[i] << std::endl;
        std::cout << std::hex << "exp_ctrl[" << i << "]    = " << exp_ctrl[i] << std::endl;
        return false;
      }
    }
  }

  return true;
}

static inline bool byte_test(const ap_uint<64> test_size) {
  if (!test_fixture(aligner(test_size, 8))) {
    std::cout << test_size << " Byte test failed." << std::endl;
    return false;
  }
  return true;
}

int main(int argc, char const *argv[]) {

  if (!byte_test(1)) return 1;
  if (!byte_test(119)) return 1;
  if (!byte_test(120)) return 1;
  if (!byte_test(121)) return 1;
  if (!byte_test(239)) return 1;
  if (!byte_test(240)) return 1;
  if (!byte_test(241)) return 1;
  if (!byte_test(1000)) return 1;

  return 0;
}
