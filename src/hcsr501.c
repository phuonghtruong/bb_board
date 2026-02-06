#include "hcsr501.h"
#include <stdio.h>

int hcsr501_init(hcsr501_t *sensor, int chip_num, int line_num) {
  char chip_path[32];
  snprintf(chip_path, sizeof(chip_path), "/dev/gpiochip%d", chip_num);

  sensor->chip = gpiod_chip_open(chip_path);
  if (!sensor->chip) {
    perror("Open gpiochip failed");
    return -1;
  }

  sensor->line = gpiod_chip_get_line(sensor->chip, line_num);
  if (!sensor->line) {
    perror("Get gpio line failed");
    gpiod_chip_close(sensor->chip);
    return -1;
  }

  if (gpiod_line_request_input(sensor->line, "motion_sensor") < 0) {
    perror("Request input failed");
    gpiod_chip_close(sensor->chip);
    return -1;
  }

  return 0;
}

int hcsr501_read(hcsr501_t *sensor) {
  return gpiod_line_get_value(sensor->line);
}

void hcsr501_release(hcsr501_t *sensor) {
  if (sensor->line)
    gpiod_line_release(sensor->line);
  if (sensor->chip)
    gpiod_chip_close(sensor->chip);
}
