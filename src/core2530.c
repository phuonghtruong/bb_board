// this file implements the core2530 library functions
#include "core2530.h"

int core2530_init(const char *port, speed_t baud) {
  // Initialization code here
  int fd = open(port, O_RDWR | O_NOCTTY); // | O_NDELAY);
  if (fd == -1) {
    perror("Unable to open port");
    return -1; // error
  }

  struct termios options;
  tcgetattr(fd, &options);

  cfsetispeed(&options, baud);
  cfsetospeed(&options, baud);

  options.c_cflag |= (CLOCAL | CREAD);
  options.c_cflag &= ~PARENB; // No parity
  options.c_cflag &= ~CSTOPB; // 1 stop bit
  options.c_cflag &= ~CSIZE;  // Clear data bits setting
  options.c_cflag |= CS8;     // 8 data bits

  // Set to Raw mode
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_iflag &= ~(IXON | IXOFF | IXANY);
  options.c_oflag &= ~OPOST;

  tcsetattr(fd, TCSANOW, &options);

  printf("core2530 library initialized.\n");
  return fd; // success
}

int core2530_send_data(int fd, const char *data) {
  // It's good practice to ensure data is actually written to the hardware
  int len = write(fd, data, strlen(data));
  tcdrain(fd); // Wait until all output has been physically transmitted
  return len;
}

int core2530_receive_data(int fd, char *buffer, size_t length) {
  int total_bytes = 0;
  int bytes_read;
  char *ptr = buffer;

  // Read in a loop for 500ms to catch the whole response
  for (int i = 0; i < 5; i++) {
    bytes_read = read(fd, ptr, length - total_bytes - 1);
    if (bytes_read > 0) {
      total_bytes += bytes_read;
      ptr += bytes_read;
    }
    usleep(100000); // Wait 100ms between attempts
  }
  return total_bytes;
}

int core2530_at_command(int fd, const char *cmd, char *response,
                        int response_size) {
  char at_cmd[256];
  snprintf(at_cmd, sizeof(at_cmd), "%s\r\n", cmd);

  tcflush(fd, TCIFLUSH); // Clear buffer
  write(fd, at_cmd, strlen(at_cmd));
  tcdrain(fd); // Ensure it's sent

  int total_bytes = 0;
  memset(response, 0, response_size);

  // Loop for up to 2 seconds to give Zigbee time to respond
  for (int i = 0; i < 20; i++) {
    int n = read(fd, response + total_bytes, response_size - total_bytes - 1);
    if (n > 0) {
      total_bytes += n;
      // Stop if we see "OK", "ERR", or a line break after getting data
      if (strstr(response, "OK") || strstr(response, "ERR"))
        break;
    }
    usleep(100000); // 100ms pause
  }

  if (total_bytes > 0) {
    printf("[Debug] Read %d bytes\n", total_bytes);
  }
  return total_bytes;
}
