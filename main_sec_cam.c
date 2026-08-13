#include "bb_utils.h"
#include "button.h"
#include "camera.h"
#include "hcsr501.h"
#include "i2c_lcd.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// Configuration
#define VIDEO_FRAMES 200
#define MOTION_CAPTURE_DIR "/tmp/sec_cam_capture"
#define MOTION_SENSOR_PIN "P8_12"
#define SECURITY_BTN_PIN "P8_14"
#define DISPLAY_BTN_PIN "P8_7"
#define I2C_BUS_DEVICE "/dev/i2c-2"
#define ARM_STATE_FILE "/tmp/sec_cam_enabled"

typedef enum { MODE_STATUS, MODE_DATETIME } DisplayMode;

static volatile sig_atomic_t stop_requested = 0;
static CameraDevice *g_cam = NULL;
static hcsr501_t *g_sensor = NULL;
static button_t *g_sec_button = NULL;
static button_t *g_disp_button = NULL;
static int g_lcd_fd = -1;
static uint8_t g_lcd_addr = I2C_ADDR;

void request_stop(int sig) {
  (void)sig;
  stop_requested = 1;
}

void cleanup(void) {
  if (g_lcd_fd >= 0) {
    close(g_lcd_fd);
    g_lcd_fd = -1;
  }

  if (g_cam) {
    camera_stop_streaming(g_cam);
    camera_close(g_cam);
    g_cam = NULL;
  }

  if (g_sec_button) {
    button_release(g_sec_button);
    g_sec_button = NULL;
  }

  if (g_disp_button) {
    button_release(g_disp_button);
    g_disp_button = NULL;
  }

  if (g_sensor) {
    hcsr501_release(g_sensor);
    g_sensor = NULL;
  }
}

void lcd_reinit(int fd, uint8_t addr) {
  lcd_init(fd, addr);
  usleep(10000);
  lcd_clear(fd, addr);
  usleep(5000);
}

void safe_lcd_refresh(int fd, uint8_t addr, DisplayMode mode, int enabled) {
  usleep(30000);
  lcd_clear(fd, addr);
  usleep(5000);

  if (mode == MODE_STATUS) {
    lcd_put_cursor(fd, addr, 0, 0);
    lcd_send_string(fd, addr, "SECURITY: ONLINE");
    lcd_put_cursor(fd, addr, 1, 0);
    lcd_send_string(fd, addr,
                    enabled ? "MODE: ARMED     " : "MODE: DISARMED  ");
  } else {
    time_t rawtime = time(NULL);
    struct tm info;
    char d_str[17];
    char t_str[17];

    if (localtime_r(&rawtime, &info) == NULL) {
      strncpy(d_str, "DATE ERR       ", sizeof(d_str));
      strncpy(t_str, "TIME ERR       ", sizeof(t_str));
    } else {
      strftime(d_str, sizeof(d_str), "D:%Y-%m-%d    ", &info);
      strftime(t_str, sizeof(t_str), "T:%H:%M:%S     ", &info);
    }

    lcd_put_cursor(fd, addr, 0, 0);
    lcd_send_string(fd, addr, d_str);
    lcd_put_cursor(fd, addr, 1, 0);
    lcd_send_string(fd, addr, t_str);
  }
}

static int read_arm_state(void) {
  FILE *f = fopen(ARM_STATE_FILE, "r");
  if (!f)
    return 1; // default armed
  int val = 1;
  if (fscanf(f, "%d", &val) != 1)
    val = 1;
  fclose(f);
  return (val != 0) ? 1 : 0;
}

static void write_arm_state(int enabled) {
  FILE *f = fopen(ARM_STATE_FILE, "w");
  if (!f)
    return;
  fprintf(f, "%d\n", enabled ? 1 : 0);
  fclose(f);
}

static int ensure_capture_directory(void) {
  if (mkdir(MOTION_CAPTURE_DIR, 0755) == 0)
    return 0;
  if (errno == EEXIST)
    return 0;
  perror("[ERROR] Create capture directory failed");
  return -1;
}

static void save_frame_to_disk(const void *frame, size_t size, int index) {
  char filename[128];
  int fd;

  snprintf(filename, sizeof(filename), "%s/motion_%03d.jpg", MOTION_CAPTURE_DIR,
           index);
  fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("[WARNING] Failed to open capture file");
    return;
  }

  if (write(fd, frame, size) != (ssize_t)size) {
    perror("[WARNING] Failed to write complete frame");
  }
  close(fd);
}

static int capture_motion_sequence(CameraDevice *cam, int lcd_fd, uint8_t lcd_addr) {
  for (int frame = 0; frame < VIDEO_FRAMES; ++frame) {
    size_t size;
    int index;
    void *data = camera_capture_frame(cam, &size, &index);
    if (!data) {
      perror("[ERROR] camera_capture_frame failed");
      return -1;
    }

    if (size > 0) {
      save_frame_to_disk(data, size, frame);
    }

    if (camera_release_frame(cam, index) < 0) {
      perror("[WARNING] camera_release_frame failed");
    }

    if ((frame % 20) == 0) {
      char progress[17];
      snprintf(progress, sizeof(progress), "CAPTURING %03d/%03d", frame,
               VIDEO_FRAMES);
      lcd_put_cursor(lcd_fd, lcd_addr, 1, 0);
      lcd_send_string(lcd_fd, lcd_addr, progress);
    }
  }
  return 0;
}

int main(void) {
  setvbuf(stdout, NULL, _IONBF, 0);
  printf("[DEBUG] System starting. Button 2 moved to P8_7\n");

  CameraDevice my_cam;
  hcsr501_t sensor;
  button_t sec_button, disp_button;
  int chip, line;
  int motion_enabled = read_arm_state();
  DisplayMode current_display = MODE_STATUS;
  int recording_active = 0;
  int last_second = -1;

  signal(SIGINT, request_stop);
  signal(SIGTERM, request_stop);
  atexit(cleanup);

  g_lcd_fd = open(I2C_BUS_DEVICE, O_RDWR);
  if (g_lcd_fd < 0 || ioctl(g_lcd_fd, I2C_SLAVE, g_lcd_addr) < 0) {
    perror("[ERROR] I2C/LCD Connection Failed");
    return 1;
  }

  lcd_reinit(g_lcd_fd, g_lcd_addr);
  lcd_send_string(g_lcd_fd, g_lcd_addr, "SYSTEM STARTING");

  if (bbb_get_gpio_coords(SECURITY_BTN_PIN, &chip, &line) != 0 ||
      button_init(&sec_button, chip, line) != 0) {
    fprintf(stderr, "[ERROR] Security button setup failed\n");
    return 1;
  }
  g_sec_button = &sec_button;

  if (bbb_get_gpio_coords(DISPLAY_BTN_PIN, &chip, &line) != 0 ||
      button_init(&disp_button, chip, line) != 0) {
    fprintf(stderr, "[ERROR] Display button setup failed\n");
    return 1;
  }
  g_disp_button = &disp_button;

  if (bbb_get_gpio_coords(MOTION_SENSOR_PIN, &chip, &line) != 0 ||
      hcsr501_init(&sensor, chip, line) != 0) {
    fprintf(stderr, "[ERROR] Motion sensor setup failed\n");
    return 1;
  }
  g_sensor = &sensor;

  if (camera_init(&my_cam, "/dev/video0") < 0) {
    fprintf(stderr, "[ERROR] Camera Init Failed\n");
    return 1;
  }
  g_cam = &my_cam;

  if (ensure_capture_directory() < 0) {
    return 1;
  }

  printf("[DEBUG] Monitoring: BTN1 (P8.14), BTN2 (P8.7), PIR (P8.12)\n");
  safe_lcd_refresh(g_lcd_fd, g_lcd_addr, current_display, motion_enabled);

  while (!stop_requested) {
    if (button_pressed(&sec_button)) {
      motion_enabled = !motion_enabled;
      write_arm_state(motion_enabled);
      printf("[EVENT] Button 1 Pressed. Motion: %d\n", motion_enabled);
      lcd_reinit(g_lcd_fd, g_lcd_addr);
      safe_lcd_refresh(g_lcd_fd, g_lcd_addr, current_display, motion_enabled);
      usleep(400000);
    }

    if (button_pressed(&disp_button)) {
      current_display =
          (current_display == MODE_STATUS) ? MODE_DATETIME : MODE_STATUS;
      printf("[EVENT] Button 2 Pressed. Switching View.\n");
      lcd_reinit(g_lcd_fd, g_lcd_addr);
      safe_lcd_refresh(g_lcd_fd, g_lcd_addr, current_display, motion_enabled);
      usleep(400000);
    }

    /* Pick up remote/web toggles from the state file so web UI can control armed */
    int file_state = read_arm_state();
    if (file_state != motion_enabled) {
      motion_enabled = file_state;
      lcd_reinit(g_lcd_fd, g_lcd_addr);
      safe_lcd_refresh(g_lcd_fd, g_lcd_addr, current_display, motion_enabled);
      usleep(200000);
    }

    if (motion_enabled && hcsr501_read(&sensor) == 1 && !recording_active) {
      recording_active = 1;
      printf("[ALERT] Motion! Recording...\n");
      lcd_clear(g_lcd_fd, g_lcd_addr);
      lcd_send_string(g_lcd_fd, g_lcd_addr, "!! MOTION !!");

      if (camera_start_streaming(&my_cam) < 0) {
        perror("[ERROR] camera_start_streaming failed");
      } else {
        if (capture_motion_sequence(&my_cam, g_lcd_fd, g_lcd_addr) < 0) {
          fprintf(stderr, "[ERROR] Motion capture sequence failed\n");
        }
        if (camera_stop_streaming(&my_cam) < 0) {
          perror("[WARNING] camera_stop_streaming failed");
        }
      }

      recording_active = 0;
      last_second = -1;
    }

    if (!recording_active) {
      time_t now = time(NULL);
      struct tm info;
      if (localtime_r(&now, &info) != NULL && info.tm_sec != last_second) {
        safe_lcd_refresh(g_lcd_fd, g_lcd_addr, current_display, motion_enabled);
        last_second = info.tm_sec;
        printf(".");
      }
    }

    usleep(50000);
  }

  printf("[INFO] Shutdown requested, cleaning up...\n");
  return 0;
}
