#include "bb_utils.h"
#include "camera.h"
#include "hcsr501.h"
#include <stdio.h>
#include <time.h> // Required for time functions
#include <unistd.h>

#define VIDEO_FRAMES 200
#define PIN_NAME "P8_12"

void get_timestamped_filename(char *buffer, size_t size) {
  time_t rawtime;
  struct tm *info;
  char time_str[32];

  time(&rawtime);
  info = localtime(&rawtime);

  // Format: Year-Month-Day_Hour-Minute-Second
  // Result: 2026-02-05_21-05-30
  strftime(time_str, sizeof(time_str), "%Y-%m-%d_%H-%M-%S", info);

  // Final filename: motion_2026-02-05_21-05-30.mjpeg
  snprintf(buffer, size, "motion_%s.mjpeg", time_str);
}

int main() {
  CameraDevice my_cam;
  hcsr501_t sensor;
  int chip, line;

  // 1. Hardware Initialization
  if (bbb_get_gpio_coords(PIN_NAME, &chip, &line) != 0) {
    fprintf(stderr, "Could not find %s\n", PIN_NAME);
    return 1;
  }
  if (hcsr501_init(&sensor, chip, line) != 0)
    return 1;

  if (camera_init(&my_cam, "/dev/video0") < 0) {
    perror("Failed to init camera");
    return 1;
  }

  printf("Security System Online. Monitoring for motion...\n");

  while (1) {
    if (hcsr501_read(&sensor) == 1) {
      char filename[128];
      get_timestamped_filename(filename, sizeof(filename));

      printf("\n[!] MOTION DETECTED at %s\n", filename);
      printf("Recording to %s...\n", filename);

      camera_start_streaming(&my_cam);
      FILE *out = fopen(filename, "wb");
      if (!out) {
        perror("Failed to open output file");
        continue;
      }

      for (int i = 0; i < VIDEO_FRAMES; i++) {
        size_t frame_size;
        int buf_index;
        void *frame_data =
            camera_capture_frame(&my_cam, &frame_size, &buf_index);

        if (frame_data) {
          fwrite(frame_data, frame_size, 1, out);
          camera_release_frame(&my_cam, buf_index);
          if (i % 20 == 0) {
            printf(".");
            fflush(stdout);
          }
        }
      }

      fclose(out);
      camera_stop_streaming(&my_cam);
      printf("\nSaved %s. Returning to standby.\n", filename);

      // Wait 5 seconds to avoid immediate re-triggering
      // and to allow the PIR sensor to go LOW
      sleep(5);
    }

    usleep(100000); // Poll 10 times per second
  }

  hcsr501_release(&sensor);
  camera_close(&my_cam);
  return 0;
}
