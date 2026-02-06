// Header files for core2530 library
// Copyright (c) 2024 Core2530 Contributors
// Licensed under the MIT License
// SPDX-License-Identifier: MIT
// Version: 1.0.0
// Date: 2024-06-15
// Author: BrianT
// Description: This file contains the declarations for the core2530 library.
#ifndef CORE2530_H
#define CORE2530_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// Default UART4 for BeagleBone Black
#define UART_PORT "/dev/ttyS4"
#define UART_BAUDRATE B38400

// Function declarations
// Initializes the core2530 library
int core2530_init(const char *port, speed_t baud);

// Send data over UART
int core2530_send_data(int fd, const char *data);

// Receive data over UART
int core2530_receive_data(int fd, char *buffer, size_t length);

// AT command mode
int core2530_at_command(int fd, const char *cmd, char *response,
                        int response_size);

#endif // CORE2530_H
