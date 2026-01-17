#ifndef CAMERA_H
#define CAMERA_H

#include <linux/videodev2.h>
#include <stddef.h>

#define WIDTH 640
#define HEIGHT 480

struct buffer {
  void *start;
  size_t length;
};

typedef struct {
  int fd;
  struct buffer *buffers;
  unsigned int n_buffers;
} CameraDevice;

// Function Prototypes
int camera_init(CameraDevice *cam, const char *device);
int camera_start_streaming(CameraDevice *cam);
void *camera_capture_frame(CameraDevice *cam, size_t *size, int *index);
int camera_release_frame(CameraDevice *cam, int index);
void camera_stop_streaming(CameraDevice *cam);
void camera_close(CameraDevice *cam);

#endif
