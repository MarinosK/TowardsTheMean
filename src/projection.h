#ifndef PROJECTION_H
#define PROJECTION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/core/noncopyable.hpp>
#include <opencv2/core.hpp>
#include "helper.h"
#include "imageBuffer.h"

// #include <thread>


class Projection : private boost::noncopyable {
private:
  GLFWwindow* projection_window;
  bool running;
  ImageBuffer& image_buffer;
  int width;
  int height;
  float ratio;
  void gl_preample(); // standard gl preample setup 
public:
  void update_frame(); // generate next frame & detection face
  inline void pause()  {running = false;}
  inline void resume() {running = true;}
  inline int get_width() const noexcept {return width;};
  inline int get_height() const noexcept {return height;};
  inline float get_ratio() const noexcept {return ratio;};
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
