#ifndef CAPTURE_H
#define CAPTURE_H

#include <string>
#include <chrono>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/optional.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <boost/core/noncopyable.hpp>
#include "properties.h"
#include "helper.h"
#include "imageBuffer.h"

// #include <thread>

struct Face {
  cv::Rect  left_eye {};
  cv::Rect  right_eye {};
  cv::Rect  face {};
};

class Capture : private boost::noncopyable {
private:
  GLFWwindow* capture_window;
  cv::VideoCapture video_capture;
  bool running;
  ImageBuffer& image_buffer;
  cv::CascadeClassifier face_cascade;
  cv::CascadeClassifier eyes_cascade;
  int window_width;
  int window_height;
  unsigned int camera_width;
  unsigned int camera_height;
  void gl_preample(); // standard gl preample setup 
  // void prepare_load_and_save_image(cv::Mat&, ...)
  boost::optional<Face> detect_face(cv::Mat&);
  inline void get_video_frame(cv::Mat& video_frame) {
#ifndef UNSAFE_OPTIMISATIONS
    if (!video_capture.isOpened())
      throw std::runtime_error("Camera not working (maybe unplugged?)");
#endif // UNSAFE_OPTIMISATIONS
    video_capture.read(video_frame);
#ifndef UNSAFE_OPTIMISATIONS
    if( !video_frame.data )
      throw std::runtime_error("Failed to fetch data from the camera");
#endif // UNSAFE_OPTIMISATIONS
  }
  // void capture_photo(cv::Mat&);
  // noise_and_mouth_detection
public:
  void update_frame(); // generate next frame & detection face
  inline unsigned int get_camera_width() const {return camera_width;}
  inline unsigned int get_camera_height() const {return camera_height;}
  inline void pause()  {running = false;}
  inline void resume() {running = true;}
  inline int get_window_width() const noexcept {return window_width;};
  inline int get_window_height() const noexcept {return window_height;};
  explicit Capture(ImageBuffer&); 
  inline ~Capture() {
    glfwDestroyWindow(capture_window);
#ifdef DEBUG
  HELPER_LOG_OUT("Capture Dtor has been called");
#endif
  }
  Capture(const Capture&&) =delete;
  Capture& operator=(const Capture&&) =delete;
};

#endif /* auto_capture_h */
