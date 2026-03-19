#include "./data_checker.hpp"

template <typename T, typename U>
static inline T aligner(const T val_be_aligned, const U val_align_to) {
  return ((val_be_aligned + (val_align_to - 1)) / val_align_to) * val_align_to;
}

static void test_fixture(
  const ap_uint<64> test_size,
  const ap_uint<64> test_pattern_data_in,
  const ap_uint<64> test_pattern_exp_in,
  const ap_uint<1>  cancel_test,
  ap_uint<64>& diff_error_bit_cnt,
  ap_uint<64>& diff_data_byte_cnt) {

  static ap_uint<64> prev_error_bit_cnt = 0;
  static ap_uint<64> prev_data_byte_cnt = 0;

  // Output
  hls::stream<test_app::Cnt::type> cnt_out;

  // Pack Stream
  hls::stream<ap_axiu<64,0,0,0>> data_in, exp_in;
  ap_axiu<64,0,0,0> data, exp;
  data.data = test_pattern_data_in;
  exp.data  = test_pattern_exp_in;
  const ap_uint<64> test_beat_num = aligner(test_size, 8) / 8;
  for (ap_uint<64> i = 0; i < test_beat_num; i++) {
    if (i == test_beat_num - 1) {
      data.last = 1; exp.last  = 1;
    } else {
      data.last = 0; exp.last  = 0;
    }
    data_in.write(data); exp_in.write(exp);
  }

  // Test Module
  for (ap_uint<64> i = 0; i < test_beat_num + 1; i++) {
    DataChecker(data_in, exp_in, cnt_out, cancel_test);
  }

  // Calculate
  const test_app::Cnt cnt = cnt_out.read();
  const auto error_bit_cnt = cnt.error_bit_cnt;
  const auto data_byte_cnt = cnt.data_byte_cnt;
  diff_error_bit_cnt  = error_bit_cnt  - prev_error_bit_cnt;
  diff_data_byte_cnt  = data_byte_cnt  - prev_data_byte_cnt;
  prev_error_bit_cnt  = error_bit_cnt;
  prev_data_byte_cnt  = data_byte_cnt;
}

static bool no_error_test_wrapper(
  const ap_uint<64> test_size,
  const ap_uint<64> test_pattern = 0x0123456789ABCDEF) {

  const auto aligned_test_size = aligner(test_size, 8);
  ap_uint<64> diff_error_bit_cnt, diff_data_byte_cnt;
  test_fixture(aligned_test_size, test_pattern, test_pattern, 0,
                diff_error_bit_cnt, diff_data_byte_cnt);

  if (diff_error_bit_cnt  != 0 ||
      diff_data_byte_cnt  != aligned_test_size) {
    std::cout << "No error test failed." << std::endl;
    std::cout << "  test_size    = " << test_size << std::endl;
    std::cout << "  test_pattern = " << test_pattern << std::endl;
    std::cout << "  diff_error_bit_cnt  = " << diff_error_bit_cnt << std::endl;
    std::cout << "    expect = " << 0 << std::endl;
    std::cout << "  diff_data_byte_cnt  = " << diff_data_byte_cnt << std::endl;
    std::cout << "    expect = " << aligned_test_size << std::endl;
    return false;
  }

  return true;
}

static bool error_test_wrapper(const ap_uint<64> test_size) {
  std::vector<ap_uint<64>> test_case_data_in;
  std::vector<ap_uint<64>> test_case_exp_in;
  std::vector<ap_uint<64>> test_case_error_bits;

  // Test case 1
  test_case_data_in.push_back(0x0000000000000000);
  test_case_exp_in.push_back (0x1111111111111111);
  test_case_error_bits.push_back(16);

  // Test case 2/
  test_case_data_in.push_back(0xFFFFFFFFFFFFFFFF);
  test_case_exp_in.push_back (0x0000000000000000);
  test_case_error_bits.push_back(64);

  for (int i = 0; i < test_case_data_in.size(); i++) {
    const auto aligned_test_size = aligner(test_size, 8);
    ap_uint<64> diff_error_bit_cnt, diff_data_byte_cnt;
    test_fixture(aligned_test_size, test_case_data_in[i], test_case_exp_in[i], 0,
                  diff_error_bit_cnt, diff_data_byte_cnt);

    if (diff_error_bit_cnt  != (aligned_test_size / 8) * test_case_error_bits[i] ||
        diff_data_byte_cnt  != aligned_test_size) {
      std::cout << "Error test failed." << std::endl;
      std::cout << "  test_size = " << test_size << std::endl;
      std::cout << "  test_pattern_data_in = " << test_case_data_in[i] << std::endl;
      std::cout << "  test_pattern_exp_in  = " << test_case_exp_in [i] << std::endl;
      std::cout << "  diff_error_bit_cnt   = " << diff_error_bit_cnt << std::endl;
      std::cout << "    expect = " << (aligned_test_size / 8) * test_case_error_bits[i] << std::endl;
      std::cout << "  diff_data_byte_cnt   = " << diff_data_byte_cnt << std::endl;
      std::cout << "    expect = " << aligned_test_size << std::endl;
      return false;
    }
  }

  return true;
}

static bool cancel_test_wrapper(
  const ap_uint<64> test_size,
  const ap_uint<64> test_pattern = 0x0123456789ABCDEF) {

  const auto aligned_test_size = aligner(test_size, 8);
  ap_uint<64> diff_error_bit_cnt, diff_data_byte_cnt;
  test_fixture(aligned_test_size, test_pattern, test_pattern, 1,
                diff_error_bit_cnt, diff_data_byte_cnt);

  if (diff_error_bit_cnt  != 0 ||
      diff_data_byte_cnt  != 0) {
    std::cout << "Cancel test failed." << std::endl;
    std::cout << "  test_size    = " << test_size << std::endl;
    std::cout << "  test_pattern = " << test_pattern << std::endl;
    std::cout << "  diff_error_bit_cnt  = " << diff_error_bit_cnt << std::endl;
    std::cout << "    expect = " << 0 << std::endl;
    std::cout << "  diff_data_byte_cnt  = " << diff_data_byte_cnt << std::endl;
    std::cout << "    expect = " << 0 << std::endl;
    return false;
  }

  return true;
}

int main(int argc, char const *argv[]) {

  if (!no_error_test_wrapper(1)) return 1;
  if (!no_error_test_wrapper(119)) return 1;
  if (!no_error_test_wrapper(120)) return 1;
  if (!no_error_test_wrapper(121)) return 1;
  if (!no_error_test_wrapper(239)) return 1;
  if (!no_error_test_wrapper(240)) return 1;
  if (!no_error_test_wrapper(241)) return 1;
  if (!no_error_test_wrapper(1000)) return 1;

  if (!error_test_wrapper(1)) return 1;
  if (!error_test_wrapper(119)) return 1;
  if (!error_test_wrapper(120)) return 1;
  if (!error_test_wrapper(121)) return 1;
  if (!error_test_wrapper(239)) return 1;
  if (!error_test_wrapper(240)) return 1;
  if (!error_test_wrapper(241)) return 1;
  if (!error_test_wrapper(1000)) return 1;

  if (!cancel_test_wrapper(1000)) return 1;

  return 0;
}
