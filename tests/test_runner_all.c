/* Auto-generated test runner configuration */

#include "unity.h"

/* Setup and Teardown functions */
void setUp(void) {
  // Setup code before each test
}

void tearDown(void) {
  // Teardown code after each test
}

/* Test declarations */
extern void test_bbb_get_gpio_coords_P8_12(void);
extern void test_bbb_get_gpio_coords_P8_31(void);
extern void test_bbb_get_gpio_coords_P9_12(void);
extern void test_bbb_get_gpio_coords_invalid_pin(void);
extern void test_bbb_get_gpio_coords_null_pin(void);
extern void test_bbb_get_gpio_coords_multiple_pins(void);
extern void test_bbb_get_gpio_coords_consistency(void);

extern void test_button_init_structure(void);
extern void test_button_struct_fields(void);
extern void test_button_read_return_type(void);
extern void test_button_pressed_logic(void);
extern void test_button_pressed_no_trigger_steady(void);
extern void test_button_init_zeroes_state(void);
extern void test_multiple_button_instances(void);

extern void test_core2530_uart_port_defined(void);
extern void test_core2530_baudrate_defined(void);
extern void test_core2530_at_cmd_enum(void);
extern void test_core2530_init_signature(void);
extern void test_core2530_send_data_format(void);
extern void test_core2530_receive_data_buffer_sizing(void);
extern void test_core2530_at_command_format(void);
extern void test_core2530_response_contains_ok(void);
extern void test_core2530_response_contains_error(void);
extern void test_core2530_response_null_terminated(void);
extern void test_core2530_command_sequence(void);

extern void test_hcsr501_struct_fields(void);
extern void test_hcsr501_init_parameters(void);
extern void test_hcsr501_read_return_values(void);
extern void test_hcsr501_multiple_sensors(void);
extern void test_hcsr501_gpio_reference_storage(void);
extern void test_hcsr501_valid_pins(void);
extern void test_hcsr501_state_transitions(void);
extern void test_hcsr501_release_function(void);

extern void test_camera_dimensions(void);
extern void test_camera_device_struct_init(void);
extern void test_camera_buffer_struct(void);
extern void test_multiple_camera_devices(void);
extern void test_camera_fd_validity(void);
extern void test_camera_buffer_allocation(void);
extern void test_camera_multiple_buffers(void);
extern void test_camera_frame_size_yuyv(void);
extern void test_camera_frame_index_validity(void);
extern void test_camera_device_path(void);

extern void test_lcd_i2c_address(void);
extern void test_lcd_line1_address(void);
extern void test_lcd_line2_address(void);
extern void test_lcd_enable_pin(void);
extern void test_lcd_backlight_pin(void);
extern void test_lcd_command_mode(void);
extern void test_lcd_data_mode(void);
extern void test_lcd_string_message(void);
extern void test_lcd_two_line_display(void);
extern void test_lcd_nibble_values(void);
extern void test_lcd_cursor_position_validity(void);
extern void test_lcd_clear_functionality(void);
extern void test_lcd_character_values(void);

extern void test_bme280_i2c_address(void);
extern void test_bme280_register_addresses(void);
extern void test_bme280_data_registers(void);
extern void test_bme280_control_registers(void);
extern void test_bme280_special_registers(void);
extern void test_bme280_return_codes(void);
extern void test_bme280_calibration_structure(void);
extern void test_bme280_calibration_fields(void);
extern void test_bme280_reset_value(void);
extern void test_bme280_register_ordering(void);
extern void test_bme280_adc_resolution(void);

extern void test_rtc_get_system_time_string_callable(void);
extern void test_rtc_buffer_size(void);
extern void test_rtc_time_format_parts(void);
extern void test_rtc_timestamp_reasonableness(void);
extern void test_rtc_buffer_parameter_handling(void);
extern void test_rtc_time_changes(void);
extern void test_rtc_date_components_valid(void);
extern void test_rtc_string_generation(void);
extern void test_rtc_gmtime_available(void);

int main(void) {
  UNITY_BEGIN();
  
  /* BB Utils Tests */
  printf("\n=== BB Utils Tests ===\n");
  RUN_TEST(test_bbb_get_gpio_coords_P8_12);
  RUN_TEST(test_bbb_get_gpio_coords_P8_31);
  RUN_TEST(test_bbb_get_gpio_coords_P9_12);
  RUN_TEST(test_bbb_get_gpio_coords_invalid_pin);
  RUN_TEST(test_bbb_get_gpio_coords_null_pin);
  RUN_TEST(test_bbb_get_gpio_coords_multiple_pins);
  RUN_TEST(test_bbb_get_gpio_coords_consistency);
  
  /* Button Tests */
  printf("\n=== Button Tests ===\n");
  RUN_TEST(test_button_init_structure);
  RUN_TEST(test_button_struct_fields);
  RUN_TEST(test_button_read_return_type);
  RUN_TEST(test_button_pressed_logic);
  RUN_TEST(test_button_pressed_no_trigger_steady);
  RUN_TEST(test_button_init_zeroes_state);
  RUN_TEST(test_multiple_button_instances);
  
  /* Core2530 Tests */
  printf("\n=== Core2530 Tests ===\n");
  RUN_TEST(test_core2530_uart_port_defined);
  RUN_TEST(test_core2530_baudrate_defined);
  RUN_TEST(test_core2530_at_cmd_enum);
  RUN_TEST(test_core2530_init_signature);
  RUN_TEST(test_core2530_send_data_format);
  RUN_TEST(test_core2530_receive_data_buffer_sizing);
  RUN_TEST(test_core2530_at_command_format);
  RUN_TEST(test_core2530_response_contains_ok);
  RUN_TEST(test_core2530_response_contains_error);
  RUN_TEST(test_core2530_response_null_terminated);
  RUN_TEST(test_core2530_command_sequence);
  
  /* HCSR501 Tests */
  printf("\n=== HCSR501 Tests ===\n");
  RUN_TEST(test_hcsr501_struct_fields);
  RUN_TEST(test_hcsr501_init_parameters);
  RUN_TEST(test_hcsr501_read_return_values);
  RUN_TEST(test_hcsr501_multiple_sensors);
  RUN_TEST(test_hcsr501_gpio_reference_storage);
  RUN_TEST(test_hcsr501_valid_pins);
  RUN_TEST(test_hcsr501_state_transitions);
  RUN_TEST(test_hcsr501_release_function);
  
  /* Camera Tests */
  printf("\n=== Camera Tests ===\n");
  RUN_TEST(test_camera_dimensions);
  RUN_TEST(test_camera_device_struct_init);
  RUN_TEST(test_camera_buffer_struct);
  RUN_TEST(test_multiple_camera_devices);
  RUN_TEST(test_camera_fd_validity);
  RUN_TEST(test_camera_buffer_allocation);
  RUN_TEST(test_camera_multiple_buffers);
  RUN_TEST(test_camera_frame_size_yuyv);
  RUN_TEST(test_camera_frame_index_validity);
  RUN_TEST(test_camera_device_path);
  
  /* LCD Tests */
  printf("\n=== LCD Tests ===\n");
  RUN_TEST(test_lcd_i2c_address);
  RUN_TEST(test_lcd_line1_address);
  RUN_TEST(test_lcd_line2_address);
  RUN_TEST(test_lcd_enable_pin);
  RUN_TEST(test_lcd_backlight_pin);
  RUN_TEST(test_lcd_command_mode);
  RUN_TEST(test_lcd_data_mode);
  RUN_TEST(test_lcd_string_message);
  RUN_TEST(test_lcd_two_line_display);
  RUN_TEST(test_lcd_nibble_values);
  RUN_TEST(test_lcd_cursor_position_validity);
  RUN_TEST(test_lcd_clear_functionality);
  RUN_TEST(test_lcd_character_values);
  
  /* BME280 Tests */
  printf("\n=== BME280 Tests ===\n");
  RUN_TEST(test_bme280_i2c_address);
  RUN_TEST(test_bme280_register_addresses);
  RUN_TEST(test_bme280_data_registers);
  RUN_TEST(test_bme280_control_registers);
  RUN_TEST(test_bme280_special_registers);
  RUN_TEST(test_bme280_return_codes);
  RUN_TEST(test_bme280_calibration_structure);
  RUN_TEST(test_bme280_calibration_fields);
  RUN_TEST(test_bme280_reset_value);
  RUN_TEST(test_bme280_register_ordering);
  RUN_TEST(test_bme280_adc_resolution);
  
  /* RTC Tests */
  printf("\n=== RTC Tests ===\n");
  RUN_TEST(test_rtc_get_system_time_string_callable);
  RUN_TEST(test_rtc_buffer_size);
  RUN_TEST(test_rtc_time_format_parts);
  RUN_TEST(test_rtc_timestamp_reasonableness);
  RUN_TEST(test_rtc_buffer_parameter_handling);
  RUN_TEST(test_rtc_time_changes);
  RUN_TEST(test_rtc_date_components_valid);
  RUN_TEST(test_rtc_string_generation);
  RUN_TEST(test_rtc_gmtime_available);
  
  return UNITY_END();
}
