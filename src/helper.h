#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <exception>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include "properties.h"

namespace helper {
  
  // exceptions
  class param_help_exception: public std::exception {
  public:
    inline param_help_exception() {};
  };
  class quit_program_exception: public std::exception {
  public:
    inline quit_program_exception() {};
  };
  class too_many_faces_exception: public std::exception {
  public:
    inline too_many_faces_exception() {};
  };
  class no_cv_data_exception: public std::exception {
  public:
    inline no_cv_data_exception() {};
  };

  // logging
  namespace logging {
    void setup();  // init boost logging
    void supress(); // used for tests only
  }
  
  // ---- MACROS ----
#define HELPER_LOG_OUT(A) std::cout << A << std::endl;	\
  BOOST_LOG_TRIVIAL(info) << A;
#define HELPER_LOG_ERR(A) std::cerr << "ERROR: " << A << std::endl;	\
  BOOST_LOG_TRIVIAL(error) << "ERROR: " << A;

  // free functions
  void parametrise(int, char**); // parametrise from command line
  std::vector<cv::Mat> loadSampleImages();
  
  // opengl
  namespace gl {
    void setup();
    inline void error_callback(int error, const char* description) {
      HELPER_LOG_ERR(description << " Error code: " << error);
      throw std::runtime_error(description); 
    }
    inline void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	glfwSetWindowShouldClose(window, GL_TRUE);
    }
    inline void clean() { // general cleanup
      glfwTerminate(); 
    }
    void display_cv_mat(const cv::Mat&, float alpha = 1.f); 
  }
}

#endif
