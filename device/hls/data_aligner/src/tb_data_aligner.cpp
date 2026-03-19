#include "./data_aligner.hpp"

template <typename T, typename U>
static inline T aligner(const T val_be_aligned, const U val_align_to) {
  return ((val_be_aligned + (val_align_to - 1)) / val_align_to) * val_align_to;
}

static bool test_fixture(
  const ap_uint<64> test_size,
  const ap_uint<64> comma_interval,
  const ap_uint<64> shift) {

  // Generate Test Data
  std::vector<ap_uint<64>> test_data;
  std::vector<ap_uint<8>>  test_ctrl;
  std::vector<ap_uint<64>> exp_data;
  const ap_uint<64> test_beat_num = aligner(test_size, 8) / 8;

  for (ap_uint<64> i = 0; i < test_beat_num; i++) {
    if ((i % comma_interval) == 0) {
      test_data.push_back(COMMA);
      test_ctrl.push_back(0x1);
    }
    // test_data.push_back(i);
    test_data.push_back(0xFFFFFFFFFFFFFFFF);
    test_ctrl.push_back(0x0);
    // exp_data.push_back(i);
    exp_data.push_back(0xFFFFFFFFFFFFFFFF);
  }

  // Shift Test Data
  std::vector<ap_uint<64>> test_data_shifted;
  std::vector<ap_uint<8>>  test_ctrl_shifted;
  for (ap_uint<64> i = 0; i < test_data.size(); i++) {
    if (i == 0) {
      test_data_shifted.push_back(test_data[i] << (shift * 8));
    } else {
      test_data_shifted.push_back((test_data[i] << (shift * 8)) | (test_data[i - 1] >> (64 - shift * 8)));
    }
    test_ctrl_shifted.push_back(test_ctrl[i] << shift);
  }
  if (shift != 0) {
    test_data_shifted.push_back(test_data[test_data.size() - 1] >> (64 - shift * 8));
    test_ctrl_shifted.push_back(test_ctrl[0] << shift);
  } else {
    // Dummy data
    test_data_shifted.push_back(0xFFFFFFFFFFFFFFFF);
    test_ctrl_shifted.push_back(0xFF);
  }

  // Pack Stream
  hls::stream<ap_uint<64>> data_in;
  hls::stream<ap_uint<8>>  ctrl_in;
  hls::stream<ap_uint<64>> aligned_data_out;
  for (ap_uint<64> i = 0; i < test_data_shifted.size(); i++) {
    data_in.write(test_data_shifted[i]);
    ctrl_in.write(test_ctrl_shifted[i]);
  }

  // Test Module
  for (ap_uint<64> i = 0; i < test_data_shifted.size(); i++) {
    DataAligner(data_in, ctrl_in, aligned_data_out);
  }

  // Unpack Stream
  std::vector<ap_uint<64>> output;
  while (!aligned_data_out.empty()) {
    output.push_back(aligned_data_out.read());
  }

  // Check Data
  if (output.size() != exp_data.size()) {
    std::cout << std::hex << "Output size does not match" << std::endl;
    std::cout << std::hex << "shift = " << shift << std::endl;
    std::cout << std::hex << "output.size()   = " << output.size() << std::endl;
    std::cout << std::hex << "exp_data.size() = " << exp_data.size() << std::endl;
    return false;
  } else {
    for (ap_uint<64> i = 0; i < output.size(); i++) {
      if (output[i] != exp_data[i]) {
        std::cout << std::hex << "Data does not match" << std::endl;
        std::cout << std::hex << "shift = " << shift << std::endl;
        std::cout << std::hex << "output[" << i << "]   = " << output[i] << std::endl;
        std::cout << std::hex << "exp_data[" << i << "] = " << exp_data[i] << std::endl;
        return false;
      }
    }
  }

  return true;
}

static inline bool test_wrapper(
  const ap_uint<64> test_size,
  const ap_uint<64> comma_interval,
  const ap_uint<64> shift) {

  if (!test_fixture(test_size, comma_interval, shift)) {
    std::cout << "Test failed." << std::endl;
    std::cout << "  test_size = "      << test_size << std::endl;
    std::cout << "  comma_interval = " << comma_interval << std::endl;
    std::cout << "  shift = "          << shift << std::endl;
    return false;
  }
  return true;
}

int main(int argc, char const *argv[]) {

  for (int shift = 0; shift < 8; shift++) {
    if (!test_wrapper(1, 15, shift)) return 1;
    if (!test_wrapper(119, 15, shift)) return 1;
    if (!test_wrapper(120, 15, shift)) return 1;
    if (!test_wrapper(121, 15, shift)) return 1;
    if (!test_wrapper(239, 15, shift)) return 1;
    if (!test_wrapper(240, 15, shift)) return 1;
    if (!test_wrapper(241, 15, shift)) return 1;
    if (!test_wrapper(1000, 15, shift)) return 1;
  }

  return 0;
}
