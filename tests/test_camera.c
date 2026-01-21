#include "camera.h"
#include "unity.h"
#include <stdatomic.h>

void setUp(void) {
  // This is run before EACH TEST
}

void tearDown(void) {
  // This is run after EACH TEST
}

void test_camera_initialization(void) {
  CameraDevice cam;
  int result = camera_init(&cam, "/dev/video0");
  TEST_ASSERT_EQUAL(0, result);
  camera_close(&cam);
}

void test_camera_start_streaming(void) {
  CameraDevice cam;
  camera_init(&cam, "/dev/video0");
  int result = camera_start_streaming(&cam);
  TEST_ASSERT_EQUAL(0, result);
  camera_stop_streaming(&cam);
  camera_close(&cam);
}

void test_camera_capture_and_release_frame(void) {
  CameraDevice cam;
  camera_init(&cam, "/dev/video0");
  camera_start_streaming(&cam);

  size_t frame_size;
  int index;
  void *frame = camera_capture_frame(&cam, &frame_size, &index);
  TEST_ASSERT_NOT_NULL(frame);
  TEST_ASSERT_GREATER_THAN(0, frame_size);

  int release_result = camera_release_frame(&cam, index);
  TEST_ASSERT_EQUAL(0, release_result);

  camera_stop_streaming(&cam);
  camera_close(&cam);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_camera_initialization);
  RUN_TEST(test_camera_start_streaming);
  RUN_TEST(test_camera_capture_and_release_frame);
  return UNITY_END();
}
