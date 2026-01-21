SRC_DIR = src
INC_DIR = include

# Added: path where Unity test scripts are located
TEST_DIR = tests
UNITY_DIR = $(TEST_DIR)/unity

# define all possible output binaries
ALL_TARGETS = bb_main bb_camera_app

CC = arm-linux-gnueabihf-gcc
CFLAGS = -I $(INC_DIR) -I$(UNITY_DIR) -Wall
LDFLAGS =
# Added: -lv4l2 for video4linux2 support
LIBS = -li2c

# --- Debug Logic --- 
#  Usage:
#  		'make'						-> Normal build
#  		'make DEBUG=1' 		-> Build with debug messages
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG_MODE -g
else
	CFLAGS += -O2  # Optimization level 2 
endif

# --- App Selection Logic ---
#  usage:
#  		'make APP=main' 		-> Build with main
#  		'make APP=camera DEBUG=1' -> Build with app2 and debug messages
ifeq ($(APP), camera)
	ENTRY_POINT = main_sec_cam.c
	TARGET = bb_camera_app
	LDFLAGS += -L/usr/lib/arm-linux-gnueabihf
	LIBS += -lv4l2 -lpthread -static-libgcc
else
	Entry_POINT = main.c
	TARGET = bb_main
	LDFLAGS += -static
endif

#  source discovery
SRCS = $(ENTRY_POINT) $(wildcard $(SRC_DIR)/*.c)

TEST_SRCS = $(filter-out $(ENTRY_POINT), $(SRCS)) \
						$(wildcard $(TEST_DIR)/*.c) $(wildcard $(UNITY_DIR)/*.c)

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS) $(LIBS)

test:
	$(CC) $(CFLAGS) $(TEST_SRCS) -o test_runner $(LDFLAGS) $(LIBS)
	@echo "Build successful. Run ./test_runner to execute tests."
	@echo "----------------------------------------"
	./test_runner

clean:
	rm -f $(ALL_TARGETS) test_runner
