#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main() {
  const char *port = "/dev/ttyS1";
  
  printf("=== UART Diagnostics for %s ===\n\n", port);

  // 1. Check if device exists
  printf("1. Checking device file...\n");
  if (access(port, F_OK) == 0) {
    printf("   ✓ Device file exists\n");
  } else {
    printf("   ✗ Device file NOT found\n");
    return 1;
  }

  // 2. Check if device is readable/writable
  printf("\n2. Checking permissions...\n");
  if (access(port, R_OK) == 0) {
    printf("   ✓ Device is readable\n");
  } else {
    printf("   ✗ Device is NOT readable\n");
  }
  
  if (access(port, W_OK) == 0) {
    printf("   ✓ Device is writable\n");
  } else {
    printf("   ✗ Device is NOT writable\n");
  }

  // 3. Try to open the device
  printf("\n3. Opening device...\n");
  int fd = open(port, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror("   ✗ Failed to open");
    return 1;
  }
  printf("   ✓ Device opened (fd=%d)\n", fd);

  // 4. Get current serial settings
  printf("\n4. Reading current UART settings...\n");
  struct termios options;
  if (tcgetattr(fd, &options) < 0) {
    perror("   ✗ tcgetattr failed");
    close(fd);
    return 1;
  }
  printf("   ✓ Current settings read\n");
  printf("   - c_cflag: 0x%x\n", options.c_cflag);
  printf("   - c_iflag: 0x%x\n", options.c_iflag);
  printf("   - c_oflag: 0x%x\n", options.c_oflag);
  printf("   - c_lflag: 0x%x\n", options.c_lflag);

  // 5. Try to set baud rate
  printf("\n5. Configuring UART to 115200 baud...\n");
  cfsetispeed(&options, B115200);
  cfsetospeed(&options, B115200);
  
  options.c_cflag |= (CLOCAL | CREAD);
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_iflag &= ~(IXON | IXOFF | IXANY);
  options.c_oflag &= ~OPOST;
  
  options.c_cc[VTIME] = 10;
  options.c_cc[VMIN] = 0;

  if (tcsetattr(fd, TCSANOW, &options) < 0) {
    perror("   ✗ tcsetattr failed");
    close(fd);
    return 1;
  }
  printf("   ✓ UART configured\n");

  // 6. Flush buffers
  printf("\n6. Flushing buffers...\n");
  tcflush(fd, TCIOFLUSH);
  printf("   ✓ Buffers flushed\n");

  // 7. Try sending AT command
  printf("\n7. Sending test AT command...\n");
  const char *test_cmd = "AT\r\n";
  int sent = write(fd, test_cmd, strlen(test_cmd));
  if (sent < 0) {
    perror("   ✗ Write failed");
    close(fd);
    return 1;
  }
  printf("   ✓ Sent %d bytes: %s", sent, test_cmd);
  tcdrain(fd);

  // 8. Try reading response
  printf("\n8. Waiting for response (2 seconds)...\n");
  char buffer[256];
  memset(buffer, 0, sizeof(buffer));
  int total_bytes = 0;

  for (int i = 0; i < 20; i++) {
    usleep(100000); // 100ms
    int n = read(fd, buffer + total_bytes, sizeof(buffer) - total_bytes - 1);
    
    if (n > 0) {
      total_bytes += n;
      buffer[total_bytes] = '\0';
      printf("   ✓ Received %d bytes (iteration %d)\n", n, i);
      printf("     Data: %s\n", buffer);
      
      if (strstr(buffer, "OK") || strstr(buffer, "ERROR")) {
        printf("   ✓ Device responded with terminator!\n");
        break;
      }
    }
  }

  if (total_bytes == 0) {
    printf("   ✗ No data received\n");
    printf("\n   Troubleshooting steps:\n");
    printf("   1. Check UART is enabled in device tree overlays\n");
    printf("      → Run: config-pin -q P8_31 (for RX) and P8_32 (for TX)\n");
    printf("   2. Verify Core2530 module is powered and connected\n");
    printf("   3. Check serial cable for proper RX/TX connections (not crossed)\n");
    printf("   4. Test with loopback: echo 'AT' > %s and cat %s\n", port, port);
  } else {
    printf("\n   ✓ SUCCESS: Device is communicating!\n");
  }

  close(fd);
  printf("\n=== Diagnostics Complete ===\n");
  return 0;
}
