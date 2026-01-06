#include "debug.h"
#include "i2c2_bme280.h"
#include "include/i2c_lcd.h"
#include "include/rtc.h"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

void display_time_date_scrolling(int file) {
  char full_str[40];
  char window[17];
  int offset = 0;

  printf("Starting Date/Time Scroll. Press Ctrl+C to Stop.\n");

  while (1) {
    get_system_time_string(full_str);
    int len = strlen(full_str);

    // sliding window logic
    for (int i = 0; i < 16; i++) {
      window[i] = full_str[(offset + i) % len];
    }
    window[16] = '\0';

    // update line 1
    lcd_send_byte(file, I2C_ADDR, LINE1, LCD_COMMAND);
    for (int i = 0; i < 16; i++) {
      lcd_send_byte(file, I2C_ADDR, window[i], LCD_DATA);
    }

    offset = (offset + 1) % len;
    usleep(300000);
  }

  close(file);
}

void display_time_date_in_2_rows(int file) {
  char time_str[16];
  char date_str[16];

  while (1) {
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = localtime(&rawtime);

    strftime(date_str, 16, "%d/%m/%Y", info);
    strftime(time_str, 16, "%I:%M:%S %p", info);

    lcd_put_cursor(file, I2C_ADDR, 0, 0);
    lcd_send_string(file, I2C_ADDR, date_str);
    lcd_put_cursor(file, I2C_ADDR, 1, 0);
    lcd_send_string(file, I2C_ADDR, time_str);

    usleep(100000);
  }

  close(file);
}

int main() {
  int lcd_fd;
  int bme280_fd;
  char *bus_i2c2 = "/dev/i2c-2";
  char *bus_i2c1 = "/dev/i2c-1";
  char buffer[16];

  if ((lcd_fd = open(bus_i2c2, O_RDWR)) < 0) {
    perror("failed to open I2C bus");
    return 1;
  }

  if (ioctl(lcd_fd, I2C_SLAVE, I2C_ADDR) < 0) {
    perror("failed to select LCD");
    return 1;
  }

  lcd_init(lcd_fd, I2C_ADDR);

  // display_time_date_scrolling(file);

  float temperature;
  float pressure;
  float humid;
  bme280_calib_t calib = {0}; // initialize to zero

  if (bme280_init(&bme280_fd, bus_i2c1) != 0) {
    printf("Failed to initialize BME280\n");
    return -1;
  }

  DEBUG_PRINT("Starting loop...\n");

  while (1) {
    if (bme280_read_temp(bme280_fd, &calib, &temperature) == 0) {
      lcd_put_cursor(lcd_fd, I2C_ADDR, 0, 0);
      snprintf(buffer, sizeof(buffer), "T:%.1f C ", temperature);
      lcd_send_string(lcd_fd, I2C_ADDR, buffer);
    } else {
      lcd_put_cursor(lcd_fd, I2C_ADDR, 0, 0);
      snprintf(buffer, sizeof(buffer), "T: Error  ");
      lcd_send_string(lcd_fd, I2C_ADDR, buffer);
    }

    if (bme280_read_press(bme280_fd, &calib, &pressure) == 0) {
      lcd_put_cursor(lcd_fd, I2C_ADDR, 1, 0);
      snprintf(buffer, sizeof(buffer), "P:%.1f hPa ", pressure);
      lcd_send_string(lcd_fd, I2C_ADDR, buffer);
    } else {
    }

    if (bme280_read_hum(bme280_fd, &calib, &humid) == 0) {
      lcd_put_cursor(lcd_fd, I2C_ADDR, 0, 9);
      snprintf(buffer, sizeof(buffer), "H:%.1f%%", humid);
      lcd_send_string(lcd_fd, I2C_ADDR, buffer);
    }

    usleep(5000000);
  }
  close(lcd_fd);
  close(bme280_fd);
  return 0;
}
