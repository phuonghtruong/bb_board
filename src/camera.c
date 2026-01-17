#include "camera.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

int camera_init(CameraDevice *cam, const char *device) {
  cam->fd = open(device, O_RDWR);
  if (cam->fd == -1)
    return -1;

  struct v4l2_format fmt = {0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = WIDTH;
  fmt.fmt.pix.height = HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  ioctl(cam->fd, VIDIOC_S_FMT, &fmt);

  struct v4l2_requestbuffers req = {0};
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  ioctl(cam->fd, VIDIOC_REQBUFS, &req);

  cam->buffers = calloc(req.count, sizeof(struct buffer));
  cam->n_buffers = req.count;

  for (int i = 0; i < req.count; ++i) {
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    ioctl(cam->fd, VIDIOC_QUERYBUF, &buf);
    cam->buffers[i].length = buf.length;
    cam->buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, cam->fd, buf.m.offset);
  }
  return 0;
}

int camera_start_streaming(CameraDevice *cam) {
  for (int i = 0; i < cam->n_buffers; ++i) {
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (ioctl(cam->fd, VIDIOC_QBUF, &buf) == -1)
      return -1;
  }
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  return ioctl(cam->fd, VIDIOC_STREAMON, &type);
}

void *camera_capture_frame(CameraDevice *cam, size_t *size, int *index) {
  struct v4l2_buffer buf = {0};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  if (ioctl(cam->fd, VIDIOC_DQBUF, &buf) == -1)
    return NULL;
  *size = buf.bytesused;
  *index = buf.index;
  return cam->buffers[buf.index].start;
}

int camera_release_frame(CameraDevice *cam, int index) {
  struct v4l2_buffer buf = {0};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = index;
  return ioctl(cam->fd, VIDIOC_QBUF, &buf);
}

void camera_stop_streaming(CameraDevice *cam) {
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  ioctl(cam->fd, VIDIOC_STREAMOFF, &type);
}

void camera_close(CameraDevice *cam) {
  for (int i = 0; i < cam->n_buffers; ++i)
    munmap(cam->buffers[i].start, cam->buffers[i].length);
  free(cam->buffers);
  close(cam->fd);
}
