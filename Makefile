SRC_DIR = src
INC_DIR = include


CC = arm-linux-gnueabihf-gcc
CFLAGS = -I $(INC_DIR) -Wall
LDFLAGS = -static
LIBS = -li2c
TARGET = bb_main

# --- Debug Logic --- 
#  Usage:
#  		'make'						-> Normal build
#  		'make DEBUG=1' 		-> Build with debug messages
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG_MODE -g
else
	CFLAGS += -O2  # Optimization level 2 
endif

all:
	$(CC) $(CFLAGS) main.c $(SRC_DIR)/* -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
