#include "unity.h"
#include "bb_utils.h"
#include <string.h>

// Test: bbb_get_gpio_coords with valid pin P8_12
void test_bbb_get_gpio_coords_P8_12(void) {
  int chip, line;
  int result = bbb_get_gpio_coords("P8_12", &chip, &line);
  
  TEST_ASSERT_EQUAL(0, result);
  TEST_ASSERT_EQUAL(1, chip);   // GPIO1
  TEST_ASSERT_EQUAL(12, line);   // Line 12
}

// Test: bbb_get_gpio_coords with valid pin P8_31
void test_bbb_get_gpio_coords_P8_31(void) {
  int chip, line;
  int result = bbb_get_gpio_coords("P8_31", &chip, &line);
  
  TEST_ASSERT_EQUAL(0, result);
  TEST_ASSERT_EQUAL(0, chip);   // GPIO0
  TEST_ASSERT_EQUAL(10, line);   // Line 10
}

// Test: bbb_get_gpio_coords with valid pin P9_12
void test_bbb_get_gpio_coords_P9_12(void) {
  int chip, line;
  int result = bbb_get_gpio_coords("P9_12", &chip, &line);
  
  TEST_ASSERT_EQUAL(0, result);
  TEST_ASSERT_TRUE(chip >= 0 && chip <= 4);  // Valid GPIO chip
  TEST_ASSERT_TRUE(line >= 0 && line <= 31); // Valid GPIO line
}

// Test: bbb_get_gpio_coords with invalid pin
void test_bbb_get_gpio_coords_invalid_pin(void) {
  int chip, line;
  int result = bbb_get_gpio_coords("INVALID", &chip, &line);
  
  TEST_ASSERT_EQUAL(-1, result);
}

// Test: bbb_get_gpio_coords with NULL pin_name
void test_bbb_get_gpio_coords_null_pin(void) {
  int chip, line;
  int result = bbb_get_gpio_coords(NULL, &chip, &line);
  
  TEST_ASSERT_EQUAL(-1, result);
}

// Test: bbb_get_gpio_coords with multiple valid pins
void test_bbb_get_gpio_coords_multiple_pins(void) {
  const char *pins[] = {"P8_3", "P8_4", "P8_5", "P8_6"};
  
  for (int i = 0; i < 4; i++) {
    int chip, line;
    int result = bbb_get_gpio_coords(pins[i], &chip, &line);
    TEST_ASSERT_EQUAL(0, result);
  }
}

// Test: bbb_get_gpio_coords returns consistent results
void test_bbb_get_gpio_coords_consistency(void) {
  int chip1, line1, chip2, line2;
  
  bbb_get_gpio_coords("P8_12", &chip1, &line1);
  bbb_get_gpio_coords("P8_12", &chip2, &line2);
  
  TEST_ASSERT_EQUAL(chip1, chip2);
  TEST_ASSERT_EQUAL(line1, line2);
}
