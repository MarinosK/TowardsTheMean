/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#ifndef CAPTURE_H
#define CAPTURE_H

#include <string>
#include <chrono>
#include <vector>
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
#include "mar_util.h"

class Capture : private boost::noncopyable {
private:
  
  GLFWwindow* capture_window_m;
  cv::VideoCapture video_capture_m;
  bool running_m;
  Projection* projection_process_m;
  cv::CascadeClassifier face_cascade_m;
  cv::CascadeClassifier eyes_cascade_m;
  int window_width_m;
  int window_height_m;
  unsigned int camera_width_m;
  unsigned int camera_height_m;
  FTGLPixmapFont font_m;
  double capture_counter_m;
  bool photo_capture_flag_m;
  bool capture_done_flag_m;
  bool draw_frames_flag_m;
  bool face_out_of_range_msg_flag_m;
  const std::string photo_folder_path_m;
  unsigned int photo_file_counter_m;

  void load_and_save_portait(cv::Mat&, helper::opencv::Face&); 
  void render_text(const char*, double x, double y, int font_size = 56);
  void gl_preample(); // standard gl preample setup
  boost::optional<helper::opencv::Face> detect_face(cv::Mat&,bool draw_frames = true);
  void display_detect_capture_load_and_save_portrait(cv::Mat&);
  inline void get_video_frame(cv::Mat& video_frame) {
#ifndef UNSAFE_OPTIMISATIONS
    if (!video_capture_m.isOpened())
      throw std::runtime_error("Camera not working (maybe unplugged?)");
#endif // UNSAFE_OPTIMISATIONS
    video_capture_m.read(video_frame);
#ifndef UNSAFE_OPTIMISATIONS
    if( !video_frame.data )
      throw std::runtime_error("Failed to fetch data from the camera");
#endif // UNSAFE_OPTIMISATIONS
  }
 
public:
  void update_frame(); // generate next frame & detection face
  inline unsigned int get_camera_width() const {return camera_width_m;}
  inline unsigned int get_camera_height() const {return camera_height_m;}
  inline void pause()  {running_m = false;}
  inline void resume() {running_m = true;}
  inline int get_window_width() const noexcept {return window_width_m;};
  inline int get_window_height() const noexcept {return window_height_m;};
  explicit Capture(Projection*); 
  inline ~Capture() {
    glfwDestroyWindow(capture_window_m);
#ifdef DEBUG
  HELPER_LOG_OUT("Capture Dtor has been called");
#endif
  }
  Capture(const Capture&&) =delete;
  Capture& operator=(const Capture&&) =delete;
};

#endif /* auto_capture_h */
