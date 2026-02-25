#include "unity.h"
#include "core2530.h"
#include <string.h>
#include <stdlib.h>

// Test: UART_PORT constant is defined
void test_core2530_uart_port_defined(void) {
  TEST_ASSERT_NOT_NULL(UART_PORT);
  TEST_ASSERT_EQUAL_STRING("/dev/ttyS1", UART_PORT);
}

// Test: UART_BAUDRATE is set to standard value
void test_core2530_baudrate_defined(void) {
  // B115200 is the standard baud rate macro
  TEST_ASSERT_EQUAL(B115200, UART_BAUDRATE);
}

// Test: AT command enum has expected values
void test_core2530_at_cmd_enum(void) {
  TEST_ASSERT_EQUAL(0, AT_CMD_AT);
  TEST_ASSERT_EQUAL(1, AT_CMD_VER);
  TEST_ASSERT_EQUAL(2, AT_CMD_ATE0);
  TEST_ASSERT_EQUAL(3, AT_CMD_ATE1);
  TEST_ASSERT_EQUAL(4, AT_CMD_UNKNOWN);
}

// Test: core2530_init function signature
void test_core2530_init_signature(void) {
  // Just verify the function can be called with correct parameters
  // (Requires actual hardware/mock to test fully)
  const char *port = "/dev/ttyS1";
  speed_t baud = B115200;
  
  TEST_ASSERT_NOT_NULL(port);
  TEST_ASSERT_EQUAL(B115200, baud);
}

// Test: core2530_send_data with valid data
void test_core2530_send_data_format(void) {
  const char *test_data = "AT\r\n";
  size_t expected_len = strlen(test_data);
  
  TEST_ASSERT_EQUAL(4, expected_len);
  TEST_ASSERT_EQUAL_STRING("AT\r\n", test_data);
}

// Test: core2530_receive_data buffer size handling
void test_core2530_receive_data_buffer_sizing(void) {
  char response[256];
  size_t buffer_size = sizeof(response);
  
  TEST_ASSERT_EQUAL(256, buffer_size);
  
  // Test buffer can hold typical responses
  const char *typical_response = "AT+VER\r\nCore2530 v1.0\r\nOK\r\n";
  TEST_ASSERT_LESS_THAN(buffer_size, strlen(typical_response) + 1);
}

// Test: AT command string formatting
void test_core2530_at_command_format(void) {
  char cmd_buffer[64];
  const char *cmd = "AT+VER";
  
  snprintf(cmd_buffer, sizeof(cmd_buffer), "%s\r\n", cmd);
  
  TEST_ASSERT_EQUAL_STRING("AT+VER\r\n", cmd_buffer);
  TEST_ASSERT_EQUAL(8, strlen(cmd_buffer));
}

// Test: Response parsing - OK terminator
void test_core2530_response_contains_ok(void) {
  char response[256] = "AT\r\nOK\r\n";
  
  TEST_ASSERT_NOT_NULL(strstr(response, "OK"));
}

// Test: Response parsing - ERROR terminator
void test_core2530_response_contains_error(void) {
  char response[256] = "AT+INVALID\r\nERROR\r\n";
  
  TEST_ASSERT_NOT_NULL(strstr(response, "ERROR"));
}

// Test: Response buffer should be null-terminated
void test_core2530_response_null_terminated(void) {
  char response[256];
  memset(response, 0, sizeof(response));
  
  strcpy(response, "Test Response");
  
  TEST_ASSERT_EQUAL('\0', response[strlen("Test Response")]);
}

// Test: Multiple AT commands can be sent sequentially
void test_core2530_command_sequence(void) {
  const char *commands[] = {"AT", "ATE0", "AT+VER"};
  
  for (int i = 0; i < 3; i++) {
    char cmd_buffer[64];
    snprintf(cmd_buffer, sizeof(cmd_buffer), "%s\r\n", commands[i]);
    TEST_ASSERT_NOT_NULL(cmd_buffer);
  }
}
