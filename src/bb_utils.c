#include "bb_utils.h"
#include <stdio.h>
#include <string.h>

int bbb_get_gpio_coords(const char *pin_name, int *chip, int *line) {
  char command[64];
  char buffer[64];
  FILE *fp;

  // Construct the command: gpiofind P8_12
  snprintf(command, sizeof(command), "gpiofind %s", pin_name);

  // Open the command for reading
  fp = popen(command, "r");
  if (fp == NULL) {
    perror("Failed to run gpiofind");
    return -1;
  }

  // Read the output (e.g., "gpiochip0 12")
  if (fgets(buffer, sizeof(buffer), fp) != NULL) {
    // Parse the string.
    // We expect "gpiochip" followed by an int (%d), then a space, then another
    // int (%d)
    if (sscanf(buffer, "gpiochip%d %d", chip, line) == 2) {
      pclose(fp);
      return 0; // Success
    }
  }

  // If we reach here, either fgets failed or sscanf didn't find the numbers
  pclose(fp);
  return -1;
}
