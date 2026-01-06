#ifndef BME280_H
#define BME280_H

#include <stdint.h>

#define I2C_BME280_ADDR 0x77

// Registers memory map
#define REG_CALIB_TEMP 0x88
#define REG_CALIB_PRESS 0x8E
#define REG_CALIB_HUM 0xA1
#define REG_ID 0xD0    // Read only
#define REG_RESET 0xE0 // Write only - Write 0xB6 only to Copyright
#define REG_CTRL_HUM 0xF2
#define REG_STATUS 0xF3
#define REG_CTRL_MEAS 0xF4
#define REG_CONFIG 0xF5
#define REG_PRESS_MSB 0xF7
#define REG_PRESS_LSB 0xF8
#define REG_PRESS_XLSB 0xF9
#define REG_TEMP_MSB 0xFA
#define REG_TEMP_LSB 0xFB
#define REG_TEMP_XLSB 0xFC
#define REG_HUM_MSB 0xFD
#define REG_HUM_LSB 0xFE

enum BME280_RC {
  BME280_OK,
  BME280_OPEN_I2C_BUS_FAILED,
  BME280_WR_SLAVE_ADDRESS_FAILED,
  BME280_RD_ID_FAILED,
  BME280_RD_RAW_ADC_TEMP_FAILED,
  BME280_RD_RAW_ADC_PRESS_FAILED,
  BME280_RD_ADC_PRESS_FAILED,
  BME280_RD_ADC_HUM_FAILED,
  BME280_RD_REG_FAILED
};

// structure to store calibration
typedef struct {
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;
  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;
  uint8_t dig_H1;
  int16_t dig_H2;
  uint8_t dig_H3;
  uint16_t dig_H4;
  uint16_t dig_H5;
  int8_t dig_H6;
  int32_t t_fine;
} bme280_calib_t;

// function prototypes
int bme280_init(int *fd, const char *bus);
int bme280_read_temp(int fd, bme280_calib_t *calib, float *temp);
int bme280_read_press(int fd, bme280_calib_t *calib, float *press);
int bme280_read_hum(int fd, bme280_calib_t *calib, float *hum);

#endif
