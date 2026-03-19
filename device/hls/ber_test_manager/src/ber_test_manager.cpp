#include "./ber_test_manager.hpp"

void BerTestManager(
  hls::stream<test_app::Cmd::type>& cmd_out,
  hls::stream<test_app::Ack::type>& ack_in,
  hls::stream<bool>& running_out,
  hls::stream<bool>& error_out,
  const uint64_t& test_pattern_reg,
  const uint64_t& test_size_reg,
  const uint64_t& test_voltage_reg,
  const uint64_t& i2cClockSelect_reg,
  uint64_t& result_reg,
  uint64_t& error_bit_cnt_reg,
  uint64_t& data_byte_cnt_reg,
  uint64_t& latency_reg,
  uint64_t& min_power_reg,
  uint64_t& max_power_reg,
  uint64_t& sum_power_reg,
  uint64_t& sum_power2_reg,
  uint64_t& sum_count_reg
) {

#pragma HLS interface axis port=cmd_out
#pragma HLS interface axis port=ack_in
#pragma HLS interface axis port=running_out
#pragma HLS interface axis port=error_out
#pragma HLS interface mode=s_axilite port=i2cClockSelect_reg bundle=regs offset=0x0010
#pragma HLS interface mode=s_axilite port=test_voltage_reg   bundle=regs offset=0x0020
#pragma HLS interface mode=s_axilite port=test_pattern_reg   bundle=regs offset=0x1000
#pragma HLS interface mode=s_axilite port=test_size_reg      bundle=regs offset=0x1010
#pragma HLS interface mode=s_axilite port=result_reg         bundle=regs offset=0x2000
#pragma HLS interface mode=s_axilite port=error_bit_cnt_reg  bundle=regs offset=0x2010
#pragma HLS interface mode=s_axilite port=latency_reg        bundle=regs offset=0x2020
#pragma HLS interface mode=s_axilite port=min_power_reg      bundle=regs offset=0x2030
#pragma HLS interface mode=s_axilite port=max_power_reg      bundle=regs offset=0x2040
#pragma HLS interface mode=s_axilite port=sum_power_reg      bundle=regs offset=0x2050
#pragma HLS interface mode=s_axilite port=sum_power2_reg     bundle=regs offset=0x2060
#pragma HLS interface mode=s_axilite port=sum_count_reg      bundle=regs offset=0x2070
#pragma HLS interface mode=s_axilite port=data_byte_cnt_reg  bundle=regs offset=0x2080

  ap_uint<test_app::Cmd::TEST_PATTERN_WIDTH>  test_pattern   = test_pattern_reg;
  ap_uint<test_app::Cmd::TEST_SIZE_WIDTH>     test_size      = test_size_reg;
  ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH>  test_voltage   = test_voltage_reg;
  ap_uint<test_app::Cmd::I2C_CLOCK_SEL_WIDTH> i2cClockSelect = i2cClockSelect_reg;

  ap_uint<test_app::Ack::RESULT_WIDTH> result = 0xFF;
  ap_uint<test_app::Ack::ERROR_BIT_CNT_WIDTH> error_bit_cnt = 0;
  ap_uint<test_app::Ack::DATA_BYTE_CNT_WIDTH> data_byte_cnt = 0;
  ap_uint<test_app::Ack::LATENCY_WIDTH> latency = 0;
  uint16_t min_power = 0;
  uint16_t max_power = 0;
  uint64_t sum_power = 0;
  uint64_t sum_power2 = 0;
  uint32_t sum_count = 0;

  // Send cmd to test application
  cmd_out.write(test_app::Cmd(test_pattern,
                              test_size,
                              test_voltage,
                              i2cClockSelect));

  while (cmd_out.full()) { /* Do nothing*/ };
  if (cmd_out.full()) return;
  else {
    // Initialize running and error indicator
    running_out.write(true);
    error_out.write(false);
    // Wait for ack
    const test_app::Ack ack(ack_in.read());
    result = ack.result;
    error_bit_cnt = ack.error_bit_cnt;
    data_byte_cnt = ack.data_byte_cnt;
    latency = ack.latency;
    min_power = power_manager::Ack(ack.pmbus_ack).getMinPower().getBinary();
    max_power = power_manager::Ack(ack.pmbus_ack).getMaxPower().getBinary();
    sum_power = power_manager::Ack(ack.pmbus_ack).getPowerSum().getBinary();
    sum_power2= power_manager::Ack(ack.pmbus_ack).getPowPowerSum().getBinary();
    sum_count = power_manager::Ack(ack.pmbus_ack).getPowerCount();
    // Update running and error indicator
    error_out.write(ack.result != 0x0);
    while (error_out.full()) { /* Do nothing*/ };
    if (error_out.full()) return;
    else {
      running_out.write(false);
    }
  }

  result_reg = result;
  error_bit_cnt_reg = error_bit_cnt;
  data_byte_cnt_reg = data_byte_cnt;
  latency_reg = latency;
  min_power_reg = min_power;
  max_power_reg = max_power;
  sum_power_reg = sum_power;
  sum_power2_reg = sum_power2;
  sum_count_reg = sum_count;
  return;
}
