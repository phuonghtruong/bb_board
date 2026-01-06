#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  const char *chipname = "gpiochip1";
  unsigned int line_num = 28; // P9_12
  struct gpiod_chip *chip;
  struct gpiod_line *line;
  int ret;

  //. Open the GPIO chip
  chip = gpiod_chip_open_by_name(chipname);
  if (!chip) {
    perror("Open chip failed");
    return 1;
  }

  line = gpiod_chip_get_line(chip, line_num);
  if (!line) {
    perror("Get line failed");
    gpiod_chip_close(chip);
    return 1;
  }

  // 3. Request the line for output
  ret = gpiod_line_request_output(line, "blink_example", 0);
  if (ret < 0) {
    perror("Request output failed");
    gpiod_chip_close(chip);
    return 1;
  }

  printf("blinking LED on %s line %u... Press Ctrl+C to stop.\n", chipname,
         line_num);

  // 4. Loop: Set value to 1 (High) and 0 (low)
  while (1) {
    gpiod_line_set_value(line, 1);
    usleep(500000);
    gpiod_line_set_value(line, 0);
    usleep(500000);
  }

  // clean up (unreachable in this loop, but good practice)
  gpiod_line_release(line);
  gpiod_chip_close(chip);
  return 0;
}
