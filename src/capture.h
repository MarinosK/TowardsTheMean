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
  int width;
  int height;
  float ratio;
  void gl_preample(); // standard gl preample setup 
  // void prepare_load_and_save_image(cv::Mat&, ...)
  boost::optional<Face> detect_face(cv::Mat&);
  void get_video_frame(cv::Mat&);
  // void capture_photo(cv::Mat&);
  // noise_and_mouth_detection
public:
  void update_frame(); // generate next frame & detection face
  inline void pause()  {running = false;}
  inline void resume() {running = true;}
  inline int get_width() const noexcept {return width;};
  inline int get_height() const noexcept {return height;};
  inline float get_ratio() const noexcept {return ratio;};
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
