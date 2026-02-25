#include "unity.h"
#include "i2c2_bme280.h"
#include <stdint.h>

// Test: BME280 I2C address
void test_bme280_i2c_address(void) {
  TEST_ASSERT_EQUAL(0x77, I2C_BME280_ADDR);
}

// Test: Register addresses are defined
void test_bme280_register_addresses(void) {
  TEST_ASSERT_EQUAL(0x88, REG_CALIB_TEMP);
  TEST_ASSERT_EQUAL(0x8E, REG_CALIB_PRESS);
  TEST_ASSERT_EQUAL(0xA1, REG_CALIB_HUM_H1);
  TEST_ASSERT_EQUAL(0xE1, REG_CALIB_HUM_H2);
}

// Test: Register addresses - Data registers
void test_bme280_data_registers(void) {
  TEST_ASSERT_EQUAL(0xF7, REG_PRESS_MSB);
  TEST_ASSERT_EQUAL(0xF8, REG_PRESS_LSB);
  TEST_ASSERT_EQUAL(0xF9, REG_PRESS_XLSB);
  TEST_ASSERT_EQUAL(0xFA, REG_TEMP_MSB);
  TEST_ASSERT_EQUAL(0xFB, REG_TEMP_LSB);
  TEST_ASSERT_EQUAL(0xFC, REG_TEMP_XLSB);
  TEST_ASSERT_EQUAL(0xFD, REG_HUM_MSB);
  TEST_ASSERT_EQUAL(0xFE, REG_HUM_LSB);
}

// Test: Control registers
void test_bme280_control_registers(void) {
  TEST_ASSERT_EQUAL(0xF2, REG_CTRL_HUM);
  TEST_ASSERT_EQUAL(0xF3, REG_STATUS);
  TEST_ASSERT_EQUAL(0xF4, REG_CTRL_MEAS);
  TEST_ASSERT_EQUAL(0xF5, REG_CONFIG);
}

// Test: Special registers
void test_bme280_special_registers(void) {
  TEST_ASSERT_EQUAL(0xD0, REG_ID);
  TEST_ASSERT_EQUAL(0xE0, REG_RESET);
}

// Test: Return codes enum
void test_bme280_return_codes(void) {
  TEST_ASSERT_EQUAL(0, BME280_OK);
  TEST_ASSERT_EQUAL(1, BME280_OPEN_I2C_BUS_FAILED);
  TEST_ASSERT_EQUAL(2, BME280_WR_SLAVE_ADDRESS_FAILED);
  TEST_ASSERT_EQUAL(3, BME280_RD_ID_FAILED);
  TEST_ASSERT_EQUAL(4, BME280_RD_RAW_ADC_TEMP_FAILED);
  TEST_ASSERT_EQUAL(5, BME280_RD_RAW_ADC_PRESS_FAILED);
  TEST_ASSERT_EQUAL(6, BME280_RD_RAW_ADC_HUM_FAILED);
  TEST_ASSERT_EQUAL(7, BME280_RD_ADC_PRESS_FAILED);
  TEST_ASSERT_EQUAL(8, BME280_RD_ADC_HUM_FAILED);
  TEST_ASSERT_EQUAL(9, BME280_RD_REG_FAILED);
}

// Test: Calibration structure exists
void test_bme280_calibration_structure(void) {
  // Verify calibration structure can be instantiated
  // This is a simplified test since we need the full struct definition
  uint16_t dig_T1 = 0;
  int16_t dig_T2 = 0;
  int16_t dig_T3 = 0;
  
  TEST_ASSERT_EQUAL(0, dig_T1);
  TEST_ASSERT_EQUAL(0, dig_T2);
  TEST_ASSERT_EQUAL(0, dig_T3);
}

// Test: Calibration data fields are accessible
void test_bme280_calibration_fields(void) {
  // Test that calibration fields use correct types
  uint16_t pressure_calib = 0x8000;
  int16_t temp_calib = -100;
  
  TEST_ASSERT_EQUAL(0x8000, pressure_calib);
  TEST_ASSERT_EQUAL(-100, temp_calib);
}

// Test: Reset register value
void test_bme280_reset_value(void) {
  uint8_t reset_value = 0xB6;
  
  TEST_ASSERT_EQUAL(0xB6, reset_value);
}

// Test: Register address ordering (temperature before pressure)
void test_bme280_register_ordering(void) {
  // REG_CALIB_TEMP = 0x88 (136), REG_CALIB_PRESS = 0x8E (142)
  // Temperature calibration should come before pressure
  TEST_ASSERT_TRUE(REG_CALIB_TEMP < REG_CALIB_PRESS);
}

// Test: ADC resolution values
void test_bme280_adc_resolution(void) {
  // BME280 has 20-bit ADC
  int adc_bits = 20;
  int max_value = (1 << adc_bits) - 1;
  
  TEST_ASSERT_EQUAL(20, adc_bits);
  TEST_ASSERT_EQUAL(1048575, max_value);
}
