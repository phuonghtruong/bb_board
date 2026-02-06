#include "core2530.h"
#include <errno.h>

int main() {
  int core2530_fd;
  char *uart_port = UART_PORT;
  speed_t baudrate = UART_BAUDRATE;
  char response[256];

  // Initialize core2530 library
  core2530_fd = core2530_init(uart_port, baudrate);
  if (core2530_fd < 0) {
    return -1; // Initialization failed
  }

  // WAKE UP / WARM UP
  // Send a few blank AT commands to wake the module and auto-sync baud
  printf("Waking up module...\n");
  for (int i = 0; i < 3; i++) {
    core2530_at_command(core2530_fd, "AT", response, sizeof(response));
    usleep(100000);
  }

  // DISABLE ECHO
  core2530_at_command(core2530_fd, "ATE0", response, sizeof(response));

  // GET VERSION
  printf("Requesting Version...\n");
  int bytes =
      core2530_at_command(core2530_fd, "AT+VER", response, sizeof(response));

  if (bytes > 0) {
    printf("Final Response: %s\n", response);
  } else {
    printf("No response received.\n");
  }

  close(core2530_fd);
  return 0;
}
