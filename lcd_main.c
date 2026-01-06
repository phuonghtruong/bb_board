#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*
 * BEAGLEBONE PIN MAPPING
 * Formula: GPIO_NO / 32 = Chip
 * GPIO_NO % 32 = Line
 *
 * */

// Chip 0
#define P8_14_LINE  26 // GPIO 26
// Chip 1 
#define P8_12_LINE 12 // GPIO 44
#define P8_16_LINE 14 // GPIO 46
// Chip 2 
#define P8_18_LINE 1  // GPIO 65
#define P8_8_LINE 3   // GPIO 67 (RS)
#define P8_10_LINE 4  // GPIO 68 (EN)

struct gpiod_line *rs, *en, *d4, *d5, *d6, *d7;


