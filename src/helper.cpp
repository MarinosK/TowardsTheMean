/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include "helper.h"

// ============================= HELPER_INTERNAL =========================
namespace { 
  namespace helper_internal {
    // auxilary vars
    unsigned short camera;
    unsigned short animation_speed;
    unsigned short max_images_in_loop;
    unsigned int capture_screen_width;
    unsigned int capture_screen_height;
    unsigned int projection_monitor_width;
    unsigned int projection_monitor_height;
    unsigned long quit_after_minutes;
    unsigned short anti_alliasing;
    int vsync;
    GLFWmonitor* primary_monitor;
    GLFWmonitor* projection_monitor;
    float new_image_fadein_time;
    // descriptions for boost::program_options
    boost::program_options::options_description init_descriptions() {
      boost::program_options::options_description desc("Allowed options");
      desc.add_options()
	("help", "produce help message")
	("animation_speed", boost::program_options::value<unsigned short>
	 (&helper_internal::animation_speed) ->default_value(ANIMATION_SPEED),
	 "set animation speed")
	("camera", boost::program_options::value<unsigned short>(&helper_internal::camera)
	 ->default_value(CAMERA_INDEX), "select camera (integer index)")
	("max_images_in_loop",boost::program_options::value<unsigned short>
	 (&helper_internal::max_images_in_loop)->default_value(MAX_IMAGES_IN_LOOP),
	 "set maximum number of images in loop")
	("quit_after_minutes",boost::program_options::value<unsigned long>
	 (&helper_internal::quit_after_minutes)->default_value(QUIT_AFTER_MINUTES),
	 "auto-quit the application after that many minutes")
	("vsync",boost::program_options::value<int>(&helper_internal::vsync)->default_value(VSYNC),
	 "vsync is the buffer swapping interval")
	("anti_alliasing",boost::program_options::value<unsigned short>
	 (&helper_internal::anti_alliasing)->default_value(ANTI_ALLIASING),
	 "anti-alliasing settings")
	("new_image_fadein_time", boost::program_options::value<float>
	 (&helper_internal::new_image_fadein_time)->default_value(NEW_IMAGE_FADEIN_TIME),
	 "set fade-in timw (seconds) for new images");
      return desc;
    }
  }
}

// ============================= runtime properties ============================== 
// these are defined as references to internal variables which are set in parametrise
const unsigned short& properties::camera {helper_internal::camera};
const unsigned short& properties::animation_speed {helper_internal::animation_speed};
const unsigned short& properties::max_images_in_loop {helper_internal::max_images_in_loop};
const float& properties::new_image_fadein_time {helper_internal::new_image_fadein_time};
const unsigned int& properties::capture_screen_width {helper_internal::capture_screen_width};
const unsigned int& properties::capture_screen_height {helper_internal::capture_screen_height};
const int& properties::vsync {helper_internal::vsync};
const unsigned short& properties::anti_alliasing {helper_internal::anti_alliasing};
const unsigned int& properties::projection_monitor_width {
  helper_internal::projection_monitor_width};
const unsigned int& properties::projection_monitor_height {
  helper_internal::projection_monitor_height};
const unsigned long& properties::quit_after_minutes {helper_internal::quit_after_minutes};
GLFWmonitor*& properties::primary_monitor {helper_internal::primary_monitor};
GLFWmonitor*& properties::projection_monitor {helper_internal::projection_monitor};


// ============================= logging ============================== 
void helper::logging::setup() {
  constexpr auto r_size = 10 * 1024 * 1024;
  boost::log::add_file_log
    (boost::log::keywords::file_name = LOGGING_FILE_PATH,
     boost::log::keywords::rotation_size = r_size,
     boost::log::keywords::format = "[%TimeStamp%]: %Message%"
     );
  boost::log::add_common_attributes();
}
void helper::logging::supress() {
  boost::log::core::get()->set_filter
    (boost::log::trivial::severity > boost::log::trivial::fatal);
}

// ============================= parametrise ============================== 
void helper::parametrise(int ac, char** argv) { // set runtime constants
  auto desc = helper_internal::init_descriptions(); // get descriptions
  // set variables nap
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(ac, argv, desc),vm);
  boost::program_options::notify(vm);
  // print descriptions on --help
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    throw helper::param_help_exception(); // to quit from main
  }
  // log otherwise
  HELPER_LOG_OUT( "--animation_speed set to " << properties::animation_speed );
  HELPER_LOG_OUT( "--using camera " << properties::camera );
  HELPER_LOG_OUT( "--maximum number of images in animation loop set to "
		  << properties::max_images_in_loop );
  HELPER_LOG_OUT( "--fade-in time for new images set to " << properties::new_image_fadein_time );
  HELPER_LOG_OUT( "--vsync set to " << properties::vsync);
  HELPER_LOG_OUT( "--anti-alliasing set to " << properties::anti_alliasing);
  HELPER_LOG_OUT( "--the application will quit after " << properties::quit_after_minutes
		  << " minutes");
#ifdef UNSAFE_OPTIMISATIONS
  HELPER_LOG_OUT( "*** UNSAFE OPTIMISATIONS TURNED ON ***");
#endif // UNSAFE_OPTIMISATIONS
}

// ============================= load sample images ============================== 
std::vector<cv::Mat> helper::loadSampleImages() {
  std::vector<cv::Mat> images {};
  auto path = properties::root_path;
  path /= properties::sample_images_path;
  std::for_each(boost::filesystem::directory_iterator{path},
		boost::filesystem::directory_iterator{},
		[&](boost::filesystem::directory_entry file){
		  cv::Mat image{cv::imread(file.path().string())};
		  cv::resize(image,image,cv::Size{CAPTURED_IMAGE_WIDTH,CAPTURED_IMAGE_HEIGHT});  // this might not be necessary later on
		  images.emplace_back(image);
		});
  return images;    
}

// ============================= opengl ============================== 
void helper::gl::setup() {
  // setup glfw
  if( !glfwInit() ) throw std::runtime_error("Failed to initialize GLFW"); 
  glfwSetErrorCallback(helper::gl::error_callback); // register error_callback
  glfwWindowHint(GLFW_SAMPLES, properties::anti_alliasing); // anti-alliasing
  // retrieve monitors and update properties
  int count;
  GLFWmonitor** monitors = glfwGetMonitors(&count);
  if (count != 2)
    throw std::runtime_error("Two monitors are necessary to run this software!");
  helper_internal::primary_monitor = monitors[0];
  helper_internal::projection_monitor = monitors[1];
  const GLFWvidmode* primary_mode {glfwGetVideoMode(properties::primary_monitor)};
  const GLFWvidmode* projection_mode {glfwGetVideoMode(properties::projection_monitor)};
  helper_internal::capture_screen_width = primary_mode->width;
  helper_internal::capture_screen_height = primary_mode->height;
  HELPER_LOG_OUT( "--capture screen resolution detected: " << properties::capture_screen_width
  		  << "x" << properties::capture_screen_height);
  helper_internal::projection_monitor_width = projection_mode->width;
  helper_internal::projection_monitor_height = projection_mode->height;
  HELPER_LOG_OUT( "--projection screen resolution detected: " << properties::projection_monitor_width
  		  << "x" << properties::projection_monitor_height);
}
void helper::gl::display_cv_mat(const cv::Mat& mat, float alpha) { 
  if (mat.data) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);               
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, mat.ptr());
    glColor4f(1,1,1,alpha);
    glBegin (GL_QUADS);
    glTexCoord2d(0.f,0.f);
    glVertex2d(0.f,1.f);
    glTexCoord2d(1.f,0.f); 
    glVertex2d(1.f,1.f);
    glTexCoord2d(1.f,1.f); 
    glVertex2d(1.f,0.f);
    glTexCoord2d(0.f,1.f); 
    glVertex2d(0.f,0.f);
    glEnd();
    glDeleteTextures( 1, &texture ); // **** clean up
  } else throw helper::no_cv_data_exception();
}
