#include "core2530.h"

int main() {
  int core2530_fd;
  char *uart_port = UART_PORT;
  speed_t baudrate = UART_BAUDRATE;
  char response[256];

  printf("Using UART port: %s at 115200 baud\n", uart_port);

  // Initialize core2530 library
  core2530_fd = core2530_init(uart_port, baudrate);
  if (core2530_fd < 0) {
    fprintf(stderr, "Failed to initialize core2530\n");
    return -1; // Initialization failed
  }

  // WAKE UP / WARM UP
  // Send a few blank AT commands to wake the module and auto-sync baud
  printf("\nWaking up module...\n");
  for (int i = 0; i < 3; i++) {
    printf("Wake attempt %d/3\n", i + 1);
    core2530_at_command(core2530_fd, "AT", response, sizeof(response));
    usleep(200000);
  }

  // DISABLE ECHO
  printf("\nDisabling echo...\n");
  core2530_at_command(core2530_fd, "ATE0", response, sizeof(response));

  // GET VERSION
  printf("\nRequesting Version...\n");
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
