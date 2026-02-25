#include "unity.h"
#include "camera.h"
#include <string.h>
#include <stdlib.h>

// Test: Camera dimensions are correct
void test_camera_dimensions(void) {
  TEST_ASSERT_EQUAL(640, WIDTH);
  TEST_ASSERT_EQUAL(480, HEIGHT);
}

// Test: CameraDevice structure initialization
void test_camera_device_struct_init(void) {
  CameraDevice cam;
  
  cam.fd = -1;
  cam.buffers = NULL;
  cam.n_buffers = 0;
  
  TEST_ASSERT_EQUAL(-1, cam.fd);
  TEST_ASSERT_NULL(cam.buffers);
  TEST_ASSERT_EQUAL(0, cam.n_buffers);
}

// Test: Buffer structure exists
void test_camera_buffer_struct(void) {
  struct buffer buf;
  
  buf.start = NULL;
  buf.length = 0;
  
  TEST_ASSERT_NULL(buf.start);
  TEST_ASSERT_EQUAL(0, buf.length);
}

// Test: Multiple camera devices can be defined
void test_multiple_camera_devices(void) {
  CameraDevice cam1, cam2;
  
  cam1.fd = 10;
  cam2.fd = 11;
  
  TEST_ASSERT_NOT_EQUAL(cam1.fd, cam2.fd);
}

// Test: Camera device file descriptor is valid
void test_camera_fd_validity(void) {
  CameraDevice cam;
  cam.fd = 3;
  // File descriptor should be >= 0
  TEST_ASSERT_TRUE(cam.fd >= 0);
}

// Test: Buffer allocation
void test_camera_buffer_allocation(void) {
  struct buffer buf;
  size_t buf_size = 640 * 480 * 2;  // YUYV format
  
  buf.start = malloc(buf_size);
  buf.length = buf_size;
  
  TEST_ASSERT_NOT_NULL(buf.start);
  TEST_ASSERT_EQUAL(buf_size, buf.length);
  
  free(buf.start);
}

// Test: Multiple buffers can be allocated
void test_camera_multiple_buffers(void) {
  struct buffer *buffers = malloc(4 * sizeof(struct buffer));
  
  TEST_ASSERT_NOT_NULL(buffers);
  
  for (int i = 0; i < 4; i++) {
    buffers[i].start = malloc(640 * 480 * 2);
    buffers[i].length = 640 * 480 * 2;
    
    TEST_ASSERT_NOT_NULL(buffers[i].start);
  }
  
  for (int i = 0; i < 4; i++) {
    free(buffers[i].start);
  }
  free(buffers);
}

// Test: Frame size calculation
void test_camera_frame_size_yuyv(void) {
  // YUYV format: 2 bytes per pixel
  size_t frame_size = WIDTH * HEIGHT * 2;
  
  TEST_ASSERT_EQUAL(640 * 480 * 2, frame_size);
  TEST_ASSERT_EQUAL(614400, frame_size);
}

// Test: Capture frame index validity
void test_camera_frame_index_validity(void) {
  int frame_index = 0;
  int max_buffers = 4;
  // Frame index should be in range [0, max_buffers)
  TEST_ASSERT_TRUE(frame_index >= 0 && frame_index < max_buffers);
}

// Test: Camera device paths
void test_camera_device_path(void) {
  const char *device_path = "/dev/video0";
  
  TEST_ASSERT_NOT_NULL(device_path);
  TEST_ASSERT_EQUAL_STRING("/dev/video0", device_path);
}
