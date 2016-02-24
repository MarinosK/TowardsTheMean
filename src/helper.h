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
#include <algorithm>
#include <exception>
#include <iterator>
#include <cstddef>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <Poco/Net/NetException.h>
#include "properties.h"

// ---- MACROS ----
#define HELPER_LOG_OUT(A) std::cout << A << std::endl; BOOST_LOG_TRIVIAL(info) << A;
#define HELPER_LOG_ERR(A) std::cerr << "ERROR: " << A << std::endl; BOOST_LOG_TRIVIAL(error) << "ERROR: " << A;

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
    inline void supress() { // used for tests only
        boost::log::core::get()->set_filter(boost::log::trivial::severity> boost::log::trivial::fatal);
    }
  }
  
  // free functions
  void parametrise(int, char**); // parametrise from command line
  void parametrise_average(int, char**, std::string& input_path); // for average command line tool
  std::vector<cv::Mat> loadSampleImages();

  // algorithm borrowed from my algo library
  // folds a container by means of applying op to pairs recursively (associativeness is not standard)
  template<class It, class Func>
  auto binary_fold(It begin, It end, Func op) ->  decltype(op(*begin, *end)) {
    std::ptrdiff_t diff = end - begin;
    switch (diff) {
      case 0: throw std::out_of_range("binary fold on empty container");
      case 1: return *begin;
      case 2: return op(*begin, *(begin + 1));
      default: { // first round to the nearest multiple of 2 and then advance
  	It mid{begin};
  	int div = diff/2;
  	int offset = (div%2 == 1) ? (div+1) : div; // round to the closest multiple of two (upwards)
  	std::advance(mid, offset);
  	return op( binary_fold(begin,mid,op), binary_fold(mid,end,op) );
      }
    }
  }

  // bot
  namespace bot {
    void prepare_photo(const std::string& path);
    void generate_average(const std::string& path_to_folder, const std::string& output_file);
    std::string generate_unique_filename_for_average();
    void send_email(const std::string& path);
  }
  
  // opencv
  namespace opencv {
    struct Face {
      cv::Rect left_eye {};
      cv::Rect right_eye {};
      cv::Rect face {};
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
    inline void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
    }
    inline void clean() { glfwTerminate(); }
    void display_cv_mat(const cv::Mat&);
  }
}

#endif
