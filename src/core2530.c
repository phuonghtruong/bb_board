// this file implements the core2530 library functions
#include "core2530.h"

int core2530_init(const char *port, speed_t baud) {
  // Initialization code here
  int fd = open(port, O_RDWR | O_NOCTTY);
  if (fd == -1) {
    perror("Unable to open port");
    return -1; // error
  }

  struct termios options;
  if (tcgetattr(fd, &options) < 0) {
    perror("tcgetattr failed");
    close(fd);
    return -1;
  }

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
  
  // Set read timeout: 1 second timeout (VTIME is in tenths of seconds)
  options.c_cc[VTIME] = 10;  // 1 second timeout
  options.c_cc[VMIN] = 0;    // Return immediately if any data available

  if (tcsetattr(fd, TCSANOW, &options) < 0) {
    perror("tcsetattr failed");
    close(fd);
    return -1;
  }

  // Flush any stale data
  tcflush(fd, TCIOFLUSH);

  printf("core2530 library initialized on %s at %d baud.\n", port, baud);
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

  printf("[AT] Sending: %s", at_cmd);

  // Clear input/output buffers
  tcflush(fd, TCIOFLUSH);
  
  // Send the AT command
  int sent = write(fd, at_cmd, strlen(at_cmd));
  if (sent < 0) {
    perror("[AT] write failed");
    return -1;
  }
  printf("[AT] Sent %d bytes\n", sent);
  tcdrain(fd); // Ensure it's sent before reading
  
  // Initial delay to let module start responding
  usleep(100000); // 100ms delay

  int total_bytes = 0;
  memset(response, 0, response_size);
  int consecutive_empty_reads = 0;

  // Loop for up to 3 seconds to collect the response
  for (int i = 0; i < 30; i++) {
    int n = read(fd, response + total_bytes, response_size - total_bytes - 1);
    
    if (n > 0) {
      consecutive_empty_reads = 0;
      total_bytes += n;
      response[total_bytes] = '\0'; // Null terminate
      printf("[AT] Read %d bytes (iteration %d)\n", n, i);
      
      // Stop if we see "OK" or "ERROR" (end of response)
      if (strstr(response, "OK") || strstr(response, "ERROR")) {
        printf("[AT] Found terminator\n");
        break;
      }
    } else {
      consecutive_empty_reads++;
      // If we have data and got 2 empty reads in a row, consider it done
      if (total_bytes > 0 && consecutive_empty_reads >= 2) {
        printf("[AT] No more data after %d reads\n", consecutive_empty_reads);
        break;
      }
    }
    
    usleep(100000); // Wait 100ms between reads
  }

  // Final null termination
  response[total_bytes] = '\0';

  if (total_bytes > 0) {
    printf("[AT] Total: %d bytes, Response: %s\n", total_bytes, response);
  } else {
    printf("[AT] No data received\n");
  }
  return total_bytes;
}
