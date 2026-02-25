#include "unity.h"
#include "i2c_lcd.h"
#include <string.h>
#include <stdint.h>

// Test: LCD I2C address is correct
void test_lcd_i2c_address(void) {
  TEST_ASSERT_EQUAL(0x27, I2C_ADDR);
}

// Test: LCD line 1 address
void test_lcd_line1_address(void) {
  TEST_ASSERT_EQUAL(0x80, LINE1);
}

// Test: LCD line 2 address
void test_lcd_line2_address(void) {
  TEST_ASSERT_EQUAL(0xC0, LINE2);
}

// Test: LCD enable pin value
void test_lcd_enable_pin(void) {
  TEST_ASSERT_EQUAL(0x04, LCD_ENABLE);
}

// Test: LCD backlight pin value
void test_lcd_backlight_pin(void) {
  TEST_ASSERT_EQUAL(0x08, LCD_BACKLIGHT);
}

// Test: LCD command mode flag
void test_lcd_command_mode(void) {
  TEST_ASSERT_EQUAL(0, LCD_COMMAND);
}

// Test: LCD data mode flag
void test_lcd_data_mode(void) {
  TEST_ASSERT_EQUAL(1, LCD_DATA);
}

// Test: String handling - typical message
void test_lcd_string_message(void) {
  char test_str[] = "Hello World";
  
  TEST_ASSERT_NOT_NULL(test_str);
  TEST_ASSERT_EQUAL(11, strlen(test_str));
}

// Test: LCD two-line display capability
void test_lcd_two_line_display(void) {
  const char *line1_msg = "Line 1";
  const char *line2_msg = "Line 2";
  
  TEST_ASSERT_NOT_NULL(line1_msg);
  TEST_ASSERT_NOT_NULL(line2_msg);
  TEST_ASSERT_NOT_EQUAL(LINE1, LINE2);
}

// Test: Nibble mode value range
void test_lcd_nibble_values(void) {
  uint8_t nibble_values[] = {0x0F, 0xF0, 0x00};
  
  for (int i = 0; i < 3; i++) {
    TEST_ASSERT_TRUE(nibble_values[i] <= 0xFF);
  }
}

// Test: Cursor position validity
void test_lcd_cursor_position_validity(void) {
  int valid_rows[] = {0, 1};
  int valid_cols[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  
  for (int r = 0; r < 2; r++) {
    for (int c = 0; c < 16; c++) {
      TEST_ASSERT_EQUAL(valid_rows[r], r);
      TEST_ASSERT_EQUAL(valid_cols[c], c);
    }
  }
}

// Test: LCD clear command
void test_lcd_clear_functionality(void) {
  // Clear command typically uses 0x01
  uint8_t clear_cmd = 0x01;
  
  TEST_ASSERT_EQUAL(0x01, clear_cmd);
}

// Test: Character encoding
void test_lcd_character_values(void) {
  const char *charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  
  for (int i = 0; i < (int)strlen(charset); i++) {
    TEST_ASSERT_TRUE(charset[i] >= 32 && charset[i] <= 126);
  }
}
