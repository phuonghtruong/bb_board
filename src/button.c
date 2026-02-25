#include "button.h"
#include <stdio.h>

int button_init(button_t *button, int chip_num, int line_num) {
  char chip_path[32];
  snprintf(chip_path, sizeof(chip_path), "/dev/gpiochip%d", chip_num);

  button->chip = gpiod_chip_open(chip_path);
  if (!button->chip) {
    perror("Open gpiochip failed");
    return -1;
  }

  button->line = gpiod_chip_get_line(button->chip, line_num);
  if (!button->line) {
    perror("Get gpio line failed");
    gpiod_chip_close(button->chip);
    return -1;
  }

  if (gpiod_line_request_input(button->line, "control_button") < 0) {
    perror("Request input failed");
    gpiod_chip_close(button->chip);
    return -1;
  }

  button->last_state = 0;
  return 0;
}

int button_read(button_t *button) {
  return gpiod_line_get_value(button->line);
}

int button_pressed(button_t *button) {
  int current_state = gpiod_line_get_value(button->line);
  
  // Detect rising edge (button press: 0 -> 1)
  // On BBB with pull-ups, LOW (0) = pressed, HIGH (1) = not pressed
  // So we detect falling edge (1 -> 0) as actual press
  if (button->last_state == 1 && current_state == 0) {
    button->last_state = current_state;
    return 1;  // Button was just pressed
  }
  
  button->last_state = current_state;
  return 0;  // No press detected
}

void button_release(button_t *button) {
  if (button->line)
    gpiod_line_release(button->line);
  if (button->chip)
    gpiod_chip_close(button->chip);
}
