#include "camera.h"
#include <stdio.h>

int main() {
  CameraDevice my_cam;

  if (camera_init(&my_cam, "/dev/video0") < 0) {
    perror("Failed to init camera");
    return 1;
  }

  camera_start_streaming(&my_cam);
  FILE *out = fopen("security_feed.mjpeg", "wb");

  printf("Streaming... Press Ctrl+C to stop.\n");

  for (int i = 0; i < 200; i++) {
    size_t frame_size;
    int buf_index;
    void *frame_data = camera_capture_frame(&my_cam, &frame_size, &buf_index);

    if (frame_data) {
      fwrite(frame_data, frame_size, 1, out);
      camera_release_frame(&my_cam, buf_index);
      printf(".");
      fflush(stdout);
    }
  }

  printf("\nFinished. Cleaning up...\n");
  fclose(out);
  camera_stop_streaming(&my_cam);
  camera_close(&my_cam);

  // convert to mjpeg into mp4 format

  return 0;
}
