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
// #include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/core/noncopyable.hpp>
#include <opencv2/core.hpp>
#include "helper.h"
#include "imageBuffer.h"

class Projection : private boost::noncopyable {
private:
  GLFWwindow* projection_window;
  bool running;
  ImageBuffer image_buffer; 
  int window_width;
  int window_height;
  std::queue<cv::Mat> images_to_fade_in;
  double fade_counter;
  bool fade_in_done;
  bool fade_out_done;
  void fade_in_new_images(cv::Mat);
  void gl_preample(); // standard gl preample setup 
public:
  void update_frame(); // generate next frame & detection face
  inline void pause()  {running = false;}
  inline void resume() {running = true;}
  inline int get_window_width() const noexcept {return window_width;};
  inline int get_window_height() const noexcept {return window_height;};
  inline void add_to_the_animation(const cv::Mat& mat) { images_to_fade_in.emplace(mat); }
  Projection(); 
  inline ~Projection() {
    glfwDestroyWindow(projection_window);
#ifdef DEBUG
    HELPER_LOG_OUT("Projection Dtor has been called");
#endif
  }
  Projection(const Projection&&) =delete;
  Projection& operator=(const Projection&&) =delete;
};

#endif
