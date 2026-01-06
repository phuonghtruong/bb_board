#include "i2c2_bme280.h"
#include "debug.h"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

// Internal helper: Write a register
static int bme280_write_reg(int fd, uint8_t reg, uint8_t val) {
  uint8_t buf[2] = {reg, val};
  return write(fd, buf, 2);
}

// Internal helper: Read a register
static int bme280_read_regs(int fd, uint8_t reg, uint8_t *data, int len) {
  if (write(fd, &reg, 1) != 1)
    return BME280_RD_REG_FAILED;
  return read(fd, data, len);
}

int bme280_init(int *fd, const char *bus) {
  DEBUG_PRINT("Initialize BME280...\n");
  // 1. Open I2C bus
  if ((*fd = open(bus, O_RDWR)) < 0)
    return BME280_OPEN_I2C_BUS_FAILED;

  // 2. Set Slave Address
  if (ioctl(*fd, I2C_SLAVE, I2C_BME280_ADDR) < 0)
    return BME280_WR_SLAVE_ADDRESS_FAILED;

  // 3. Read ID (check if sensor is actually there)
  uint8_t id;
  bme280_read_regs(*fd, REG_ID, &id, 1);
  if (id != 0x60) {
    fprintf(stderr, "Error: BME280 not found (ID: 0x%02X)\n", id);
    return BME280_RD_ID_FAILED;
  }

  // 4. Configure: Normal mode, 1x temp oversampling, 1x pressure oversampling
  bme280_write_reg(*fd, REG_CTRL_MEAS, 0x27);
  return 0;
}

int bme280_read_temp(int fd, bme280_calib_t *calib, float *temp) {
  DEBUG_PRINT("Reading temperature data...\n");
  uint8_t data[6];

  // Read Calibration Data if not already loaded (T1-T3)
  if (calib->dig_T1 == 0) {
    bme280_read_regs(fd, REG_CALIB_TEMP, data, 6);
    calib->dig_T1 = (data[1] << 8) | data[0];
    calib->dig_T2 = (int16_t)(data[3] << 8) | data[2];
    calib->dig_T3 = (int16_t)(data[5] << 8) | data[4];
  }

  // Read Raw ADC temperature
  if (bme280_read_regs(fd, REG_TEMP_MSB, data, 3) != 3)
    return BME280_RD_RAW_ADC_TEMP_FAILED;

  int32_t adc_T = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) |
                  ((uint32_t)data[2] >> 4);

  // Compensation math
  double var1, var2;
  var1 = (((double)adc_T) / 16384.0 - ((double)calib->dig_T1) / 1024.0) *
         ((double)calib->dig_T2);
  var2 = ((((double)adc_T) / 131072.0 - ((double)calib->dig_T1) / 8192.0) *
          (((double)adc_T) / 131072.0 - ((double)calib->dig_T1) / 8192.0)) *
         ((double)calib->dig_T3);

  calib->t_fine = (int32_t)(var1 + var2);

  *temp = (float)((var1 + var2) / 5120.0);
  DEBUG_PRINT("%.2f\n", *temp);
  return 0;
}

int bme280_read_press(int fd, bme280_calib_t *calib, float *press) {
  DEBUG_PRINT("Reading Pressure data...\n");
  uint8_t data[18];

  // Read Calibration Data if not already loaded (P1-P9)
  if (calib->dig_P1 == 0) {
    bme280_read_regs(fd, REG_CALIB_PRESS, data, 18);
    calib->dig_P1 = (data[1] << 8) | data[0];
    calib->dig_P2 = (data[3] << 8) | data[2];
    calib->dig_P3 = (data[5] << 8) | data[4];
    calib->dig_P4 = (data[7] << 8) | data[6];
    calib->dig_P5 = (data[9] << 8) | data[8];
    calib->dig_P6 = (data[11] << 8) | data[10];
    calib->dig_P7 = (data[13] << 8) | data[12];
    calib->dig_P8 = (data[15] << 8) | data[14];
    calib->dig_P9 = (data[17] << 8) | data[16];
  }

  // Read Raw ADC Pressure
  if (bme280_read_regs(fd, REG_PRESS_MSB, data, 3) != 3)
    return BME280_RD_RAW_ADC_PRESS_FAILED;

  int32_t adc_P = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) |
                  ((uint32_t)data[2] >> 4);

  // compensation math
  double var1, var2, p;
  var1 = ((double)calib->t_fine / 2.0) - 64000.0;
  var2 = var1 * var1 * ((double)calib->dig_P6) / 32768.0;
  var2 = var2 + var1 * ((double)calib->dig_P5) * 2.0;
  var2 = (var2 / 4.0) + (((double)calib->dig_P4) * 65536.0);
  var1 = (((double)calib->dig_P3) * var1 * var1 / 524288.0 +
          ((double)calib->dig_P2) * var1) /
         524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((double)calib->dig_P1);
  if (var1 == 0.0) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576.0 - ((double)adc_P);
  p = (p - (var2 / 4096.0)) * 6250.0 / var1;
  var1 = ((double)calib->dig_P9) * p * p / 2147483648.0;
  var2 = p * ((double)calib->dig_P8) / 32768.0;
  p = p + (var1 + var2 + ((double)calib->dig_P7)) / 16.0;

  *press = (float)(p / 100.0);
  DEBUG_PRINT("%.2f\n", *press);
  return 0;
}

int bme280_read_hum(int fd, bme280_calib_t *calib, float *hum) {
  DEBUG_PRINT("Reading Humidity data...\n");
  uint8_t data[8];

  if (calib->dig_H1 == 0) {
    bme280_read_regs(fd, REG_CALIB_HUM_H1, &calib->dig_H1, 1);
    bme280_read_regs(fd, REG_CALIB_HUM_H2, data, 7);
    calib->dig_H2 = (data[1] << 8) | data[0];
    calib->dig_H3 = data[2];
    calib->dig_H4 = (data[3] << 4) | (data[4] & 0x0F);
    calib->dig_H5 = (data[5] << 4) | data[4] >> 4;
    calib->dig_H6 = data[6];
  }

  // Read Raw ADC Humidity
  if (bme280_read_regs(fd, REG_HUM_MSB, data, 2) != 2)
    return BME280_RD_RAW_ADC_HUM_FAILED;

  int32_t adc_H = (data[0] << 8) | data[1];

  double var_H;
  var_H = ((double)calib->t_fine) - 76800.0;
  var_H = (adc_H - (((double)calib->dig_H4) * 64.0 +
                    ((double)calib->dig_H5) / 16384.0 * var_H)) *
          (((double)calib->dig_H2) / 65536.0 *
           (1.0 + ((double)calib->dig_H6) / 67108864.0 * var_H *
                      (1.0 + ((double)calib->dig_H3) / 67108864.0 * var_H)));
  var_H = var_H * (1.0 - ((double)calib->dig_H1) * var_H / 524288.0);
  if (var_H > 100.0)
    var_H = 100.0;
  else if (var_H < 0.0)
    var_H = 0.0;
  *hum = var_H;
  DEBUG_PRINT("%.2f\n", *hum);
  return 0;
}
