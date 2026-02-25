#include "bb_utils.h"
#include "button.h"
#include "camera.h"
#include "hcsr501.h"
#include "i2c_lcd.h"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

// Configuration
#define VIDEO_FRAMES 200
#define MOTION_SENSOR_PIN "P8_12"
#define SECURITY_BTN_PIN "P8_14"
#define DISPLAY_BTN_PIN "P8_7" // MOVED TO P8_7 (Confirmed "unused" in your log)
#define I2C_BUS_DEVICE "/dev/i2c-2"

typedef enum { MODE_STATUS, MODE_DATETIME } DisplayMode;

// --- Helper: Force LCD to reset its 4-bit state ---
void lcd_reinit(int fd, uint8_t addr) {
  lcd_init(fd, addr);
  usleep(10000);
  lcd_clear(fd, addr);
  usleep(5000);
}

void safe_lcd_refresh(int fd, uint8_t addr, DisplayMode mode, int enabled) {
  // Noise guard: wait 30ms before talking to LCD after a button press
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
    time_t rawtime;
    struct tm *info;
    char d_str[17], t_str[17];
    time(&rawtime);
    info = localtime(&rawtime);
    strftime(d_str, 17, "D:%Y-%m-%d    ", info);
    strftime(t_str, 17, "T:%H:%M:%S     ", info);
    lcd_put_cursor(fd, addr, 0, 0);
    lcd_send_string(fd, addr, d_str);
    lcd_put_cursor(fd, addr, 1, 0);
    lcd_send_string(fd, addr, t_str);
  }
}

int main() {
  // Disable stdout buffering for real-time logs
  setvbuf(stdout, NULL, _IONBF, 0);
  printf("[DEBUG] System starting. Button 2 moved to P8_7\n");

  CameraDevice my_cam;
  hcsr501_t sensor;
  button_t sec_button, disp_button;
  int chip, line, lcd_fd;

  int motion_enabled = 1;
  DisplayMode current_display = MODE_STATUS;
  int recording_active = 0;
  int last_second = -1;

  // 1. Setup I2C
  lcd_fd = open(I2C_BUS_DEVICE, O_RDWR);
  if (lcd_fd < 0 || ioctl(lcd_fd, I2C_SLAVE, I2C_ADDR) < 0) {
    perror("[ERROR] I2C/LCD Connection Failed");
    return 1;
  }

  // 2. Initial LCD Setup
  lcd_reinit(lcd_fd, I2C_ADDR);
  lcd_send_string(lcd_fd, I2C_ADDR, "SYSTEM STARTING");

  // 3. Initialize Pins
  bbb_get_gpio_coords(SECURITY_BTN_PIN, &chip, &line);
  printf("security pin name: %s, chip: %d, line: %d\n", SECURITY_BTN_PIN, chip,
         line);
  button_init(&sec_button, chip, line);

  bbb_get_gpio_coords(DISPLAY_BTN_PIN, &chip, &line);
  printf("display pin name: %s, chip: %d, line: %d\n", DISPLAY_BTN_PIN, chip,
         line);
  button_init(&disp_button, chip, line);

  bbb_get_gpio_coords(MOTION_SENSOR_PIN, &chip, &line);
  printf("motion sensor pin name: %s, chip: %d, line: %d\n", MOTION_SENSOR_PIN,
         chip, line);
  hcsr501_init(&sensor, chip, line);

  // 4. Initialize Camera
  if (camera_init(&my_cam, "/dev/video0") < 0) {
    printf("[ERROR] Camera Init Failed\n");
    return 1;
  }

  printf("[DEBUG] Monitoring: BTN1 (P8.14), BTN2 (P8.7), PIR (P8.12)\n");
  safe_lcd_refresh(lcd_fd, I2C_ADDR, current_display, motion_enabled);

  while (1) {
    // --- BUTTON 1: SECURITY TOGGLE ---
    if (button_pressed(&sec_button)) {
      motion_enabled = !motion_enabled;
      printf("[EVENT] Button 1 Pressed. Motion: %d\n", motion_enabled);
      // Re-sync LCD in case electrical noise caused "weird characters"
      lcd_reinit(lcd_fd, I2C_ADDR);
      safe_lcd_refresh(lcd_fd, I2C_ADDR, current_display, motion_enabled);
      usleep(400000);
    }

    // --- BUTTON 2: DISPLAY TOGGLE ---
    if (button_pressed(&disp_button)) {
      current_display =
          (current_display == MODE_STATUS) ? MODE_DATETIME : MODE_STATUS;
      printf("[EVENT] Button 2 Pressed. Switching View.\n");
      // Re-sync LCD to ensure characters are clean
      lcd_reinit(lcd_fd, I2C_ADDR);
      safe_lcd_refresh(lcd_fd, I2C_ADDR, current_display, motion_enabled);
      usleep(400000);
    }

    // --- MOTION LOGIC ---
    if (motion_enabled && hcsr501_read(&sensor) == 1 && !recording_active) {
      recording_active = 1;
      printf("[ALERT] Motion! Recording...\n");
      lcd_clear(lcd_fd, I2C_ADDR);
      lcd_send_string(lcd_fd, I2C_ADDR, "!! MOTION !!");
      camera_start_streaming(&my_cam);

      // (Capture loop goes here as per your previous code)

      camera_stop_streaming(&my_cam);
      recording_active = 0;
      last_second = -1; // Force immediate LCD update
    }

    // --- PERIODIC CLOCK UPDATE ---
    if (!recording_active) {
      time_t now = time(NULL);
      struct tm *tm_info = localtime(&now);
      if (tm_info->tm_sec != last_second) {
        safe_lcd_refresh(lcd_fd, I2C_ADDR, current_display, motion_enabled);
        last_second = tm_info->tm_sec;
        printf(".");
        fflush(stdout);
      }
    }
    usleep(50000);
  }
  return 0;
}
