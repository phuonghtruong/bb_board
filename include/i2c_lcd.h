#ifndef I2C_LCD_H
#define I2C_LCD_H

#include <stdint.h>

#define I2C_ADDR 0x27
#define LINE1 0x80
#define LINE2 0xC0

#define LCD_ENABLE 0x04
#define LCD_BACKLIGHT 0x08
#define LCD_COMMAND 0
#define LCD_DATA 1

void lcd_init(int file, uint8_t addr);
void lcd_init_2(const char *device);
void lcd_send_nibble(int file, uint8_t addr, uint8_t val, uint8_t mode);
void lcd_send_string(int file, uint8_t addr, char *str);
void lcd_put_cursor(int file, uint8_t addr, int row, int col);
void lcd_send_byte(int file, uint8_t addr, uint8_t bits, uint8_t mode);
void lcd_print_line(int file, int line, const char *str);
void lcd_clear(int file, uint8_t addr);

#endif
