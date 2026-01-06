#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG_MODE
// this version prints to the standard terminal
#define DEBUG_PRINT(fmt, ...)                                                  \
  printf("DEBUG [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif // DEBUG

#endif
