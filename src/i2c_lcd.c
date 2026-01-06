#include "i2c_lcd.h"
#include "debug.h"
#include <i2c/smbus.h>
#include <stdint.h>
#include <unistd.h>

void lcd_send_nibble(int file, uint8_t addr, uint8_t val, uint8_t mode) {
  uint8_t data = val | mode | LCD_BACKLIGHT;
  i2c_smbus_write_byte(file, data | LCD_ENABLE);
  usleep(500);
  i2c_smbus_write_byte(file, data & ~LCD_ENABLE);
  usleep(500);
}

void lcd_send_byte(int file, uint8_t addr, uint8_t val, uint8_t mode) {
  lcd_send_nibble(file, addr, val & 0xF0, mode);        // High nibble
  lcd_send_nibble(file, addr, (val << 4) & 0xF0, mode); // Low nibble
}

void lcd_init(int file, uint8_t addr) {
  DEBUG_PRINT("Initialize LCD 16x2...");
  usleep(50000);
  lcd_send_nibble(file, addr, 0x30, LCD_COMMAND);
  usleep(5000);
  lcd_send_nibble(file, addr, 0x30, LCD_COMMAND);
  usleep(1000);
  lcd_send_nibble(file, addr, 0x20, LCD_COMMAND); // set 4-bits mode
  //
  lcd_send_byte(file, addr, 0x28, LCD_COMMAND); // 2 lines, 5x8 font
  lcd_send_byte(file, addr, 0x0C, LCD_COMMAND); // Display ON, Cursor OFF
  lcd_send_byte(file, addr, 0x01, LCD_COMMAND); // Clear
  usleep(2000);
}

void lcd_put_cursor(int file, uint8_t addr, int row, int col) {
  int pos = (row == 0) ? (0x80 + col) : (0xC0 + col);
  lcd_send_byte(file, addr, pos, LCD_COMMAND);
}

void lcd_send_string(int file, uint8_t addr, char *str) {
  while (*str) {
    lcd_send_byte(file, addr, (uint8_t)(*str++), LCD_DATA);
  }
}

void lcd_clear(int file, uint8_t addr) {
  lcd_send_byte(file, addr, 0x01, LCD_COMMAND);
  usleep(2000);
}
