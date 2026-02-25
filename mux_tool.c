#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define CONTROL_MODULE_START 0x44E10000
#define CONTROL_MODULE_SIZE 0x2000

void print_usage(char *bin) {
  printf("Usage: sudo %s <offset_hex> <value_hex>\n", bin);
  printf("Example (P9.24 to UART1 TX): sudo %s 984 0\n", bin);
  printf("Example (P9.26 to UART1 RX): sudo %s 980 20\n", bin);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    print_usage(argv[0]);
    return 1;
  }

  // Parse inputs
  uint32_t offset = (uint32_t)strtol(argv[1], NULL, 16);
  uint32_t value = (uint32_t)strtol(argv[2], NULL, 16);

  // Basic safety check for AM335x control module range
  if (offset < 0x800 || offset > 0xAFF) {
    printf("Error: Offset 0x%X seems out of range for pinmuxing.\n", offset);
    return 1;
  }

  int fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 0) {
    perror("Could not open /dev/mem");
    return 1;
  }

  uint8_t *control_map = mmap(NULL, CONTROL_MODULE_SIZE, PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd, CONTROL_MODULE_START);
  if (control_map == MAP_FAILED) {
    perror("mmap failed");
    close(fd);
    return 1;
  }

  volatile uint32_t *reg = (volatile uint32_t *)(control_map + offset);

  printf("Changing Offset 0x%X: [Old: 0x%X] -> [New: 0x%X]\n", offset, *reg,
         value);
  *reg = value;

  munmap(control_map, CONTROL_MODULE_SIZE);
  close(fd);
  return 0;
}
