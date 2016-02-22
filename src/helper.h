/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <exception>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include "properties.h"

#include "mar_algo.h"
#include "mar_util.h"

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
  class failed_to_allign_image_exception: public std::exception {
  public:
    inline failed_to_allign_image_exception() {};
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
  void parametrise_average(int, char**, std::string& input_path); // for average command line tool
  std::vector<cv::Mat> loadSampleImages();

  // bot
  namespace bot {
    void prepare_photo(const std::string& path);
    void generate_average(const std::string& path_to_folder, const std::string& output_file);
    std::string generate_unique_filename_for_average();
    // void tweet(std::string path);
    // void send_email(std::string path);
  }
  
  // opencv
  namespace opencv {
    struct Face {
      cv::Rect left_eye {};
      cv::Rect right_eye {};
      cv::Rect face {};
      // cv::Rect mouth {};
    };
    void allign_and_isolate_face(cv::Mat&, helper::opencv::Face&);
  }

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
    void display_cv_mat(const cv::Mat&);
  }
}

#endif
