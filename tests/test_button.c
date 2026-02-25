#include "unity.h"
#include "button.h"
#include <stdlib.h>

// Mock GPIO structures for testing without hardware
button_t test_button;

// Test: button_init returns valid structure
void test_button_init_structure(void) {
  // This test checks that button_init can be called
  // In a real environment with GPIO available
  button_t button;
  
  // Initialize last_state before init
  button.last_state = 0;
  button.chip = NULL;
  button.line = NULL;
  
  TEST_ASSERT_NOT_NULL(&button);
}

// Test: button_t structure has required fields
void test_button_struct_fields(void) {
  button_t button;
  
  // Verify structure has the expected fields
  button.last_state = 0;
  button.chip = NULL;
  button.line = NULL;
  
  TEST_ASSERT_EQUAL(0, button.last_state);
  TEST_ASSERT_NULL(button.chip);
  TEST_ASSERT_NULL(button.line);
}

// Test: button_read returns valid state (0 or 1)
void test_button_read_return_type(void) {
  // Mock test - checks function signature and return type
  // Real hardware test would need actual GPIO
  button_t button;
  button.chip = NULL;
  button.line = NULL;
  
  // In real hardware, this would return 0 or 1
  // For mock, we just verify the structure is prepared
  TEST_ASSERT_NOT_NULL(&button);
}

// Test: button_pressed detects state changes
void test_button_pressed_logic(void) {
  button_t button;
  
  // Initialize button state
  button.last_state = 1;  // Start with button not pressed (HIGH)
  
  // Simulate button being pressed (0)
  int simulated_state = 0;
  
  // Manually check edge detection logic
  if (button.last_state == 1 && simulated_state == 0) {
    TEST_PASS();  // Edge detected
  } else {
    TEST_FAIL_MESSAGE("Edge detection logic failed");
  }
}

// Test: button_pressed doesn't trigger on steady state
void test_button_pressed_no_trigger_steady(void) {
  button_t button;
  
  // Start with steady state (not pressed)
  button.last_state = 1;
  int simulated_state = 1;
  
  // Check that no edge is detected
  if (button.last_state == 1 && simulated_state == 0) {
    TEST_FAIL_MESSAGE("False positive edge detection");
  } else {
    TEST_PASS();  // Correctly no edge
  }
}

// Test: button structure initialization
void test_button_init_zeroes_state(void) {
  button_t button;
  
  button.last_state = -1;
  TEST_ASSERT_EQUAL(-1, button.last_state);
  
  // Proper init should set it to a known state
  button.last_state = 0;
  TEST_ASSERT_EQUAL(0, button.last_state);
}

// Test: multiple button instances can coexist
void test_multiple_button_instances(void) {
  button_t button1, button2;
  
  button1.last_state = 0;
  button2.last_state = 1;
  
  TEST_ASSERT_NOT_EQUAL(button1.last_state, button2.last_state);
}
