#include "./test_app_base.hpp"

static bool test_fixture(
  const ap_uint<test_app::Cmd::TEST_PATTERN_WIDTH>  test_pattern,
  const ap_uint<test_app::Cmd::TEST_SIZE_WIDTH>     test_size,
  const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH>  test_voltage,
  const ap_uint<test_app::Cmd::I2C_CLOCK_SEL_WIDTH> i2cClockSelect,
  const ap_uint<test_app::Ack::ERROR_BIT_CNT_WIDTH> error_bit_cnt_result,
  const ap_uint<1>                                  cancel_test,
  const ap_uint<test_app::Ack::LATENCY_WIDTH>       latency_result) {

  // Output
  hls::stream<ap_axiu<64,0,0,0>> data_out;
  hls::stream<test_app::Ack::type> ack_out;
  hls::stream<power_manager::Cmd::type> pmbus_cmd_out;
  hls::stream<ap_uint<8>> counter_clear_out;
  hls::stream<ap_uint<1>> reset_modules_out;
  ap_uint<2> i2cClockSelect_out;

  // Input
  hls::stream<test_app::Cmd::type> cmd_in;
  hls::stream<power_manager::Ack::type> pmbus_ack_in;
  hls::stream<ap_uint<64>> counter_val_in;
  hls::stream<test_app::Cnt::type> cnt_in;

  // Calculate actual test size (aligned to 8 Byte)
  const ap_uint<test_app::Cmd::TEST_SIZE_WIDTH> actual_test_size
    = ((test_size + 7) / 8) * 8;

  // Set values
  const ap_uint<test_app::Ack::DATA_BYTE_CNT_WIDTH> data_byte_cnt_result = test_size;
  const auto cmd = test_app::Cmd(test_pattern, test_size, test_voltage, i2cClockSelect);
  cmd_in.write(cmd);
  const power_manager::Ack pmbus_ack_clear_state = power_manager::Ack(0);
  const power_manager::Ack pmbus_ack_set_under_voltage = power_manager::Ack(0);
  const power_manager::Ack pmbus_ack_set_power_good_off = power_manager::Ack(0);
  const power_manager::Ack pmbus_ack_set_power_good_on = power_manager::Ack(0);
  const power_manager::Ack pmbus_ack_set_voltage = power_manager::Ack(0);
  const power_manager::Ack pmbus_ack_start_monitoring = power_manager::Ack(0);
  const power_manager::Ack pmbus_ack_stop_monitoring = power_manager::Ack(0);
  const power_manager::Ack pmbus_ack_reset_voltage = power_manager::Ack(0);
  pmbus_ack_in.write(pmbus_ack_clear_state);
  pmbus_ack_in.write(pmbus_ack_set_under_voltage);
  pmbus_ack_in.write(pmbus_ack_set_power_good_off);
  pmbus_ack_in.write(pmbus_ack_set_power_good_on);
  pmbus_ack_in.write(pmbus_ack_set_voltage);
  pmbus_ack_in.write(pmbus_ack_start_monitoring);
  pmbus_ack_in.write(pmbus_ack_stop_monitoring);
  pmbus_ack_in.write(pmbus_ack_reset_voltage);
  const ap_uint<64> wait_cycle = 125000000 / 5;
  counter_val_in.write(1000);
  counter_val_in.write(1001);
  counter_val_in.write(wait_cycle - 1);
  counter_val_in.write(wait_cycle);
  counter_val_in.write(1000);
  counter_val_in.write(1001);
  counter_val_in.write(wait_cycle - 1);
  counter_val_in.write(wait_cycle);
  cnt_in.write(test_app::Cnt(error_bit_cnt_result, data_byte_cnt_result));
  const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH> lower_voltage_limit = 410; // 410 is 0.1 V
  const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH> under_voltage          = test_voltage - lower_voltage_limit;
  const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH> power_good_on_voltage  = under_voltage;
  const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH> power_good_off_voltage = power_good_on_voltage - 41; // 41 is 0.01 V

  // Run target
  TestAppBase(
    data_out, cmd_in, ack_out, pmbus_cmd_out, pmbus_ack_in, counter_clear_out, counter_val_in,
    reset_modules_out, cnt_in, cancel_test, latency_result, i2cClockSelect_out);

  // Check i2cClockSelect_out
  if (i2cClockSelect_out != i2cClockSelect) {
    std::cout << "i2cClockSelect_out is not correct." << std::endl;
    std::cout << "i2cClockSelect_out = " << i2cClockSelect_out << std::endl;
    return false;
  }

  // Check reset_modules_out
  if (reset_modules_out.size() != 1) {
    std::cout << "reset_modules_out is not asserted." << std::endl;
    std::cout << "reset_modules_out.size() = " << reset_modules_out.size() << std::endl;
    return false;
  } else {
    reset_modules_out.read();
  }

  // Check counter_clear_out
  if (counter_clear_out.size() != 2) {
    std::cout << "counter_clear_out is not asserted." << std::endl;
    std::cout << "counter_clear_out.size() = " << counter_clear_out.size() << std::endl;
    return false;
  } else {
    counter_clear_out.read();
    counter_clear_out.read();
  }

  // Check pmbus_cmd_out
  constexpr int pmbus_cmd_num = 8;
  const power_manager::lane_t target = 6;     // MGTAVCC
  const ap_uint<test_app::Cmd::TEST_VOLTAGE_WIDTH> default_voltage = 0x1000; // 1 V
  const uint16_t monitoring_interval_ms = 1;  // Monitoring per 1 ms
  for (int i = 0; i < pmbus_cmd_num; i++) {
    if (pmbus_cmd_out.size() != (pmbus_cmd_num - i)) {
      std::cout << "pmbus_cmd_out is not asserted." << std::endl;
      std::cout << "pmbus_cmd_out.size() = " << pmbus_cmd_out.size() << std::endl;
      return false;
    } else {
      switch (i) {
        case 0: { // clearState
          const auto  pmbus_cmd = pmbus_cmd_out.read();
          if (pmbus_cmd != power_manager::Cmd::type(power_manager::Cmd::clearState())) {
            std::cout << "pmbus_cmd is not clearState" << std::endl;
            std::cout << "pmbus_cmd = " << pmbus_cmd << std::endl;
            return false;
          }
          break;
        }
        case 1: { // setUnderVoltage
          const auto  pmbus_cmd = pmbus_cmd_out.read();
          if (pmbus_cmd != power_manager::Cmd::type(power_manager::Cmd::setUnderVoltage(target, under_voltage))) {
            std::cout << "pmbus_cmd is not setUnderVoltage" << std::endl;
            std::cout << "pmbus_cmd = " << pmbus_cmd << std::endl;
            return false;
          }
          break;
        }
        case 2: { // setPowerGoodOff
          const auto  pmbus_cmd = pmbus_cmd_out.read();
          if (pmbus_cmd != power_manager::Cmd::type(power_manager::Cmd::setPowerGoodOff(target, power_good_off_voltage))) {
            std::cout << "pmbus_cmd is not setPowerGoodOff" << std::endl;
            std::cout << "pmbus_cmd = " << pmbus_cmd << std::endl;
            return false;
          }
          break;
        }
        case 3: { // setPowerGoodOn
          const auto  pmbus_cmd = pmbus_cmd_out.read();
          if (pmbus_cmd != power_manager::Cmd::type(power_manager::Cmd::setPowerGoodOn(target, power_good_on_voltage))) {
            std::cout << "pmbus_cmd is not setPowerGoodOn" << std::endl;
            std::cout << "pmbus_cmd = " << pmbus_cmd << std::endl;
            return false;
          }
          break;
        }
        case 4: { // setVoltage
          const auto  pmbus_cmd = pmbus_cmd_out.read();
          if (pmbus_cmd != power_manager::Cmd::type(power_manager::Cmd::setVoltage(target, test_voltage))) {
            std::cout << "pmbus_cmd is not setVoltage" << std::endl;
            std::cout << "pmbus_cmd = " << pmbus_cmd << std::endl;
            return false;
          }
          break;
        }
        case 5: { // startMonitoring
          const auto  pmbus_cmd = pmbus_cmd_out.read();
          if (pmbus_cmd != power_manager::Cmd::type(power_manager::Cmd::startMonitoring(target, monitoring_interval_ms))) {
            std::cout << "pmbus_cmd is not startMonitoring" << std::endl;
            std::cout << "pmbus_cmd = " << pmbus_cmd << std::endl;
            return false;
          }
          break;
        }
        case 6: { // stopMonitoring
          const auto  pmbus_cmd = pmbus_cmd_out.read();
          if (pmbus_cmd != power_manager::Cmd::type(power_manager::Cmd::stopMonitoring())) {
            std::cout << "pmbus_cmd is not stopMonitoring" << std::endl;
            std::cout << "pmbus_cmd = " << pmbus_cmd << std::endl;
            return false;
          }
          break;
        }
        case 7: { // resetVoltage
          const auto  pmbus_cmd = pmbus_cmd_out.read();
          if (pmbus_cmd != power_manager::Cmd::type(power_manager::Cmd::setVoltage(target, default_voltage))) {
            std::cout << "pmbus_cmd is not resetVoltage" << std::endl;
            std::cout << "pmbus_cmd = " << pmbus_cmd << std::endl;
            return false;
          }
          break;
        }
        default:
          break;
      }
    }
  }

  // Check data_out
  if (cancel_test) {
    if (data_out.size() != 0) {
      std::cout << "data_out is not zero when cancel_test asserted." << std::endl;
      std::cout << "data_out.size() = " << data_out.size() << std::endl;
      return false;
    }
  } else {
    if (data_out.size() != (actual_test_size + 7) / 8) {
      std::cout << "data_out is not asserted." << std::endl;
      std::cout << "data_out.size() = " << data_out.size() << std::endl;
      return false;
    } else {
      while (data_out.size() != 0) {
        const auto data = data_out.read();
        if (data.last == data_out.size()) {
          std::cout << "data.last is invalid." << std::endl;
          std::cout << "data.last = " << data.last << std::endl;
          return false;
        }
      }
    }
  }

  // Check ack_out
  if (ack_out.size() != 1) {
    std::cout << "ack_out is not asserted." << std::endl;
    std::cout << "ack_out.size() = " << ack_out.size() << std::endl;
    return false;
  } else {
    const auto ack = ack_out.read();
    if (ack != test_app::Ack::type(test_app::Ack(
                                    (cancel_test)? 0xC: 0,
                                    error_bit_cnt_result,
                                    data_byte_cnt_result,
                                    latency_result,
                                    pmbus_ack_stop_monitoring))) {
      std::cout << "ack is not stopMonitoring" << std::endl;
      std::cout << "ack = " << ack << std::endl;
      return false;
    }
  }

  return true;
}

int main(int argc, char const *argv[]) {

  // Scenario test
  if (!test_fixture(1, 1000, 0x1000, 2, 0x0, 0, 0xF)) {
    std::cout << "Scenario test failed." << std::endl;
    return 1;
  }

  // Cancel test
  if (!test_fixture(1, 1000, 0x1000, 2, 0x0, 1, 0xF)) {
    std::cout << "Cancel test failed." << std::endl;
    return 1;
  }

  return 0;
}
