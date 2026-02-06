#ifndef BB_UTILS_H
#define BB_UTILS_H

/**
 * @brief Finds the chip and line numbers for a given header pin name.
 *
 * @param pin_name String like "P8_12"
 * @param chip Pointer to store the detected chip number
 * @param line Pointer to store the detected line number
 * @return int 0 on success, -1 if the pin was not found.
 */
int bbb_get_gpio_coords(const char *pin_name, int *chip, int *line);

#endif // BB_UTILS_H
