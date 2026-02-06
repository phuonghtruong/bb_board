#include "bb_utils.h"
#include "hcsr501.h"
#include <stdio.h>
#include <unistd.h>

int main() {
  hcsr501_t sensor;
  int last_state = -1; // Initialize to -1 so the first reading always prints
  int current_state;
  const char *sensor_gpio = "P8_12";
  int chip_num, line_num;

  printf("Searching for hardware address of %s...\n", sensor_gpio);

  // Dynamic Discovery!
  if (bbb_get_gpio_coords(sensor_gpio, &chip_num, &line_num) != 0) {
    fprintf(stderr,
            "Error: Could not find pin %s. Is it configured as a GPIO?\n",
            sensor_gpio);
    return 1;
  }
  printf("Found %s at chip %d, line %d.\n", sensor_gpio, chip_num, line_num);

  // P8_12 = GPIO Bank 1, Line 12
  if (hcsr501_init(&sensor, chip_num, line_num) != 0) {
    return 1;
  }

  printf("HC-SR501 Initialized.\n");

  // 1. Silent Warm-up period
  printf("Sensor Warming up (approx 60s)... ");
  fflush(stdout); // Ensure the message appears immediately

  // You can change 45 to a lower number (like 5) if you just want to skip the
  // wait for testing, but 30-60 is recommended for accurate PIR readings.
  sleep(60);
  printf(
      "Ready!\n\n Monitoring for motion (will only print on status change):\n");

  while (1) {
    current_state = hcsr501_read(&sensor);

    // 2. Only print if the state has changed
    if (current_state != last_state) {
      if (current_state == 1) {
        printf("[!] MOTION DETECTED!\n");
      } else {
        printf("[ ] Motion stopped.\n");
      }

      // Update the last known state
      last_state = current_state;
    }

    // 3. Short sleep to prevent CPU hogging
    usleep(100000); // 0.1 seconds is enough for responsive detection
  }

  hcsr501_release(&sensor);
  return 0;
}
