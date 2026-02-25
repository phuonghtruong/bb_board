#ifndef BUTTON_H
#define BUTTON_H

#include <gpiod.h>

typedef struct {
  struct gpiod_chip *chip;
  struct gpiod_line *line;
  int last_state;  // Track previous state for edge detection
} button_t;

// chip_num: GPIO chip number, line_num: GPIO line number
int button_init(button_t *button, int chip_num, int line_num);

// Returns 1 if button is pressed (LOW), 0 if not pressed (HIGH)
int button_read(button_t *button);

// Returns 1 if button was just pressed (state changed from 0 to 1), 0 otherwise
int button_pressed(button_t *button);

void button_release(button_t *button);

#endif
