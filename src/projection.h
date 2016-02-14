/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#ifndef PROJECTION_H
#define PROJECTION_H

#include <cmath>
#include <functional>
#include <queue>
#include <vector>
#include <mutex>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/core/noncopyable.hpp>
#include <opencv2/core.hpp>
#include "helper.h"
#include "imageBuffer.h"

class Projection : private boost::noncopyable {
private:
  GLFWwindow* projection_window_m;
  bool running_m;
  ImageBuffer image_buffer_m; 
  int window_width_m;
  int window_height_m;
  std::queue<cv::Mat> images_to_fade_in_m;
  double fade_counter_m;
  bool fade_in_done_m;
  bool fade_out_done_m;
  std::mutex mutex_m;
  void fade_in_new_images(cv::Mat);
  void gl_preample(); // standard gl preample setup 
public:
  void update_frame(); // generate next frame & detection face (thread safe)
  inline void pause()  noexcept {running_m = false;}
  inline void resume() noexcept {running_m = true;}
  inline int get_window_width() const noexcept {return window_width_m;};
  inline int get_window_height() const noexcept {return window_height_m;};
  inline void add_to_the_animation(const cv::Mat& mat) { // thread safe
    std::lock_guard<std::mutex> lock(mutex_m);
    images_to_fade_in_m.emplace(mat);
  }
  Projection(); // thread safe
  inline ~Projection() { 
    glfwDestroyWindow(projection_window_m);
#ifdef DEBUG
    HELPER_LOG_OUT("Projection Dtor has been called");
#endif
  }
  Projection(const Projection&&) =delete;
  Projection& operator=(const Projection&&) =delete;
};

#endif
