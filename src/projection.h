#ifndef PROJECTION_H
#define PROJECTION_H

#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/core/noncopyable.hpp>
#include <opencv2/core.hpp>
#include "helper.h"
#include "imageBuffer.h"

#include <functional>
// #include <thread>


class Projection : private boost::noncopyable {
private:
  GLFWwindow* projection_window;
  bool running;
  ImageBuffer& image_buffer;
  int window_width;
  int window_height;
  void gl_preample(); // standard gl preample setup 
public:
  void update_frame(); // generate next frame & detection face
  inline void pause()  {running = false;}
  inline void resume() {running = true;}
  inline int get_window_width() const noexcept {return window_width;};
  inline int get_window_height() const noexcept {return window_height;};
  explicit Projection(ImageBuffer&); 
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
