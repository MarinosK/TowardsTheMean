#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/filesystem.hpp>

// Default values for various settings and various global constants
// note that some of these values can be overriden with command line flags

// #define COLOR_MODE CV_LOAD_IMAGE_COLOR
#define QUIT_AFTER_MINUTES 600 
#define ASSETS_PATH "./assets/" 
#define FACE_CASCADE ASSETS_PATH "lbpcascade_frontalface.xml" 
#define EYES_CASCADE ASSETS_PATH "haarcascade_eye_tree_eyeglasses.xml" 
#define CAPTURE_MSG_COLOUR cv::Scalar(0,0,255) // red 
#define FACE_FRAME_COLOUR cv::Scalar(255,255,0) // yellow 
#define EYES_FRAME_COLOUR cv::Scalar(255,0,0) // blue 
#define LOGGING_FILE_PATH "./logs/%Y-%m-%d_%H-%M-%S.%N.log"
#define ANIMATION_SPEED 1 // values expected ?? 
#define CAMERA_INDEX 0 
#define MAX_IMAGES_IN_LOOP 40 
#define NEW_IMAGE_FADEIN_TIME 5 // seconds
#define VSYNC 1 // the swapping buffer interval
#define ANTI_ALLIASING 4
#define BACKGROUND_COLOUR 0.f, 0.f, 0.f, 1.f 


// Runtime global constants (externs are defined by helper::parametrise)
namespace properties {
  extern const unsigned short& camera; 
  extern const unsigned short& animation_speed;
  extern const unsigned short& max_images_in_loop;
  extern const unsigned int& capture_screen_width;
  extern const unsigned int& capture_screen_height;
  extern const unsigned int& projection_monitor_width;
  extern const unsigned int& projection_monitor_height;
  extern const unsigned long& quit_after_minutes;
  extern const float& new_image_fadein_time;
  extern const unsigned short& anti_alliasing;
  unsigned int camera_width {};   // *** set by Capture Ctor at runtime
  unsigned int camera_height {};  // *** set by Capture Ctor at runtime
  extern GLFWmonitor*& primary_monitor;     // to somehow make const sometime later
  extern GLFWmonitor*& projection_monitor;   // to somehow make const sometime later
  extern const int& vsync; 
  const boost::filesystem::path root_path {boost::filesystem::current_path()};
  const boost::filesystem::path sample_images_path {"assets/sampleImages/"};
}


#endif 
