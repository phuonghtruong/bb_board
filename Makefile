SRC_DIR = src
INC_DIR = include


CC = arm-linux-gnueabihf-gcc
CFLAGS = -I $(INC_DIR) -Wall
LDFLAGS = -static
LIBS = -li2c
TARGET = bb_main

all:
	$(CC) $(CFLAGS) main.c $(SRC_DIR)/* -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
