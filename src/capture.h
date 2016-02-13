/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#ifndef CAPTURE_H
#define CAPTURE_H

#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>
#include <cmath>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <FTGL/ftgl.h>
#include "properties.h"
#include "helper.h"
#include "projection.h"

class Capture : private boost::noncopyable {
private:
  
  GLFWwindow* capture_window;
  cv::VideoCapture video_capture;
  bool running;
  Projection* projection_process;
  cv::CascadeClassifier face_cascade;
  cv::CascadeClassifier eyes_cascade;
  int window_width;
  int window_height;
  unsigned int camera_width;
  unsigned int camera_height;
  FTGLPixmapFont font;
  double capture_counter;
  bool photo_capture_flag;
  bool thread_launched_flag;
  bool draw_frames_flag;
  std::string photo_folder_path;
  std::atomic<unsigned int> photo_file_counter;

  void load_and_save_portait(const cv::Mat&, const helper::opencv::Face&); // launches a new thread
  void render_text(const char*, double x, double y, int FontSize = 56);
  void gl_preample(); // standard gl preample setup
  boost::optional<helper::opencv::Face> detect_face(cv::Mat&,bool draw_frames = true);
  void display_detect_capture_load_and_save_portrait(cv::Mat&);
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
 
public:
  void update_frame(); // generate next frame & detection face
  inline unsigned int get_camera_width() const {return camera_width;}
  inline unsigned int get_camera_height() const {return camera_height;}
  inline void pause()  {running = false;}
  inline void resume() {running = true;}
  inline int get_window_width() const noexcept {return window_width;};
  inline int get_window_height() const noexcept {return window_height;};
  explicit Capture(Projection*); 
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
