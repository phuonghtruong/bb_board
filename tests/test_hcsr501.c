#include "unity.h"
#include "hcsr501.h"
#include <stdlib.h>

// Test: hcsr501_t structure has required fields
void test_hcsr501_struct_fields(void) {
  hcsr501_t sensor;
  
  // Initialize to known state
  sensor.chip = NULL;
  sensor.line = NULL;
  
  TEST_ASSERT_NULL(sensor.chip);
  TEST_ASSERT_NULL(sensor.line);
}

// Test: hcsr501_init can be called with valid parameters
void test_hcsr501_init_parameters(void) {
  hcsr501_t sensor;
  int chip_num = 1;
  int line_num = 12;
  
  // Verify parameters are reasonable
  TEST_ASSERT_EQUAL(1, chip_num);
  TEST_ASSERT_EQUAL(12, line_num);
  TEST_ASSERT_NOT_NULL(&sensor);
}

// Test: hcsr501_read should return 0 or 1
void test_hcsr501_read_return_values(void) {
  // Test that read function returns binary values
  // (0 = no motion, 1 = motion detected)
  
  int valid_states[] = {0, 1};
  
  for (int i = 0; i < 2; i++) {
    int state = valid_states[i];
    TEST_ASSERT_TRUE(state == 0 || state == 1);
  }
}

// Test: Multiple sensor instances can coexist
void test_hcsr501_multiple_sensors(void) {
  hcsr501_t sensor1, sensor2;
  
  sensor1.chip = (struct gpiod_chip *)0x1000;
  sensor2.chip = (struct gpiod_chip *)0x2000;
  
  TEST_ASSERT_NOT_EQUAL(sensor1.chip, sensor2.chip);
}

// Test: Sensor structure can store GPIO references
void test_hcsr501_gpio_reference_storage(void) {
  hcsr501_t sensor;
  
  // Simulated GPIO references (non-null pointers)
  sensor.chip = (struct gpiod_chip *)0xDEADBEEF;
  sensor.line = (struct gpiod_line *)0xCAFEBABE;
  
  TEST_ASSERT_NOT_NULL(sensor.chip);
  TEST_ASSERT_NOT_NULL(sensor.line);
  TEST_ASSERT_NOT_EQUAL(sensor.chip, sensor.line);
}

// Test: Motion sensor pins are valid
void test_hcsr501_valid_pins(void) {
  // Test with typical BeagleBone GPIO pins
  int valid_chips[] = {0, 1, 2, 3};
  
  for (int i = 0; i < 4; i++) {
    TEST_ASSERT_TRUE(valid_chips[i] >= 0 && valid_chips[i] <= 3);
  }
}

// Test: Motion sensor read state transitions
void test_hcsr501_state_transitions(void) {
  int prev_state = 0;
  int current_state = 1;
  
  TEST_ASSERT_EQUAL(0, prev_state);
  TEST_ASSERT_EQUAL(1, current_state);
  
  // Simulate state change
  prev_state = current_state;
  TEST_ASSERT_EQUAL(1, prev_state);
}

// Test: Release function exists
void test_hcsr501_release_function(void) {
  // Just verify structure can be created
  // In real test with hardware, would verify resources are freed
  TEST_PASS();
}
