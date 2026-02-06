#ifndef HCSR501_H
#define HCSR501_H

#include <gpiod.h>

typedef struct {
  struct gpiod_chip *chip;
  struct gpiod_line *line;
} hcsr501_t;

// chip_num: 1 for P8_12, line_num: 12 for P8_12
int hcsr501_init(hcsr501_t *sensor, int chip_num, int line_num);
int hcsr501_read(hcsr501_t *sensor);
void hcsr501_release(hcsr501_t *sensor);

#endif
