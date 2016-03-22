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
  static const std::string photo_folder_path_m;
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
  unsigned int photo_file_counter_m;

  void load_and_save_portait(cv::Mat&, helper::opencv::Face&); 
  inline void render_text(const char* text, double x, double y, int font_size = 156) { 
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPixelTransferf(GL_RED_BIAS, -1.0f);
    glPixelTransferf(GL_GREEN_BIAS, -1.0f);
    glPixelTransferf(GL_BLUE_BIAS, -1.0f);
    font_m.FaceSize(font_size);
    font_m.Render(text, -1, FTPoint(x,y));
    glPopAttrib();
  }
  inline void gl_preample() {
    glfwMakeContextCurrent(capture_window_m);    
    // glViewport(0, 0, window_width_m, window_height_m);
    glViewport(0, 0, 4096, 2304); // ***** HACK FOR RETINA DISPLAY
    glClearColor(BACKGROUND_COLOUR); 
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
  }
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
  inline void zoom_in_video_frame (cv::Mat& video_frame) {
    const unsigned int x {(camera_width_m - CAPTURED_IMAGE_WIDTH) / 2};
    const unsigned int y {(camera_height_m - CAPTURED_IMAGE_HEIGHT) / 2};
    cv::Mat roi = video_frame(cv::Rect(x,y,CAPTURED_IMAGE_WIDTH,CAPTURED_IMAGE_HEIGHT));
    roi.copyTo(video_frame);
}
 
public:
  void update_frame(); // generate next frame & detection face
  inline unsigned int get_camera_width() const {return camera_width_m;}
  inline unsigned int get_camera_height() const {return camera_height_m;}
  inline void pause()  {running_m = false;}
  inline void resume() {running_m = true;}
  inline int get_window_width() const noexcept {return window_width_m;};
  inline int get_window_height() const noexcept {return window_height_m;};
  static inline std::string get_photo_folder_path() noexcept {return photo_folder_path_m;};
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
