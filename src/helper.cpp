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
    unsigned int captured_image_width;
    unsigned int captured_image_height;
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
	("captured_image_width", boost::program_options::value<unsigned int>
	 (&helper_internal::captured_image_width)->default_value(CAPTURED_IMAGE_WIDTH),
	 "the width the captured photo should be scaled to")
	("captured_image_height", boost::program_options::value<unsigned int>
	 (&helper_internal::captured_image_height)->default_value(CAPTURED_IMAGE_HEIGHT),
	 "the height the captured photo should be scaled to")
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
const unsigned int& properties::captured_image_width {helper_internal::captured_image_width};
const unsigned int& properties::captured_image_height {helper_internal::captured_image_height};
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
  HELPER_LOG_OUT( "--the captured photos will be scaled to "
		  << properties::captured_image_width << "x" << properties::captured_image_height);
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
		  if (file.path().extension().string() == ".jpg" || file.path().extension().string() == ".tif" ||
		      file.path().extension().string() == ".tiff" || file.path().extension().string() == ".png") {
		    cv::Mat image{cv::imread(file.path().string())};
		    cv::resize(image,image,cv::Size{ // maybe not necessary in the future
			static_cast<int>(properties::captured_image_width),
			  static_cast<int>(properties::captured_image_height)});
		    images.emplace_back(image);
		  }
		});
  return images;    
}

// =============================== openCV ===============================

float helper::opencv::rms_distance_between_eyes(const Face& face)  {
  auto dx = face.right_eye.x - face.left_eye.x;
  auto dy = face.right_eye.y - face.left_eye.y;
  return std::sqrt(std::pow(dx,2) + std::pow(dy,2));
}



void helper::opencv::allign_and_isolate_face(cv::Mat& photo, helper::opencv::Face& face) {

  // add a 30% white border so that rotation/scaling will not reveal any black background (also update the face's coordinates)
  const int border_size {photo.cols / 3};
  cv::copyMakeBorder(photo, photo, border_size, border_size, border_size, border_size,
  		     cv::BORDER_REPLICATE, cv::Scalar(255,255,255));
  const cv::Point offset {border_size,border_size};
  face.left_eye += offset;
  face.right_eye += offset;
  face.face += offset;
  
  // detect the center of the eyes and a third point in the source image
  const cv::Point2f left_eye_src {face.left_eye.x + static_cast<float>(face.left_eye.width) / 2,
      face.left_eye.y + static_cast<float>(face.left_eye.height) / 2};
  const cv::Point2f right_eye_src {face.right_eye.x + static_cast<float>(face.right_eye.width) / 2,
      face.right_eye.y + static_cast<float>(face.right_eye.height) / 2};
  constexpr float sin60 {0.86f}; // sin(60 * M_PI / 180)
  constexpr float cos60 {0.49f}; // cos(60 * M_PI / 180)
  const float dx_src {left_eye_src.x - right_eye_src.x};
  const float dy_src {left_eye_src.y - right_eye_src.y};
  float third_point_src_x {cos60 * dx_src - sin60 * dy_src + right_eye_src.x};
  float third_point_src_y {sin60 * dx_src + cos60 * dy_src + right_eye_src.y};
  if (third_point_src_y > left_eye_src.y) { // if point below eye level mirror it
    third_point_src_x = cos60 * dx_src + sin60 * dy_src + right_eye_src.x;
    third_point_src_y = -sin60 * dx_src + cos60 * dy_src + right_eye_src.y;
  }
  const cv::Point2f third_point_src {third_point_src_x, third_point_src_y};

  // calculate the destination points to calculate the Affine Matrix
  constexpr float l_eye_pct {0.40f};
  constexpr float r_eye_pct {0.59f};
  constexpr float eyes_level_pct {0.43f};
  static const cv::Point2f left_eye_dst {static_cast<float>(properties::captured_image_width) * l_eye_pct, 
      static_cast<float>(properties::captured_image_height) * eyes_level_pct};
  static const cv::Point2f right_eye_dst {static_cast<float>(properties::captured_image_width) * r_eye_pct,
      static_cast<float>(properties::captured_image_height) * eyes_level_pct};
  static const float third_point_dst_x {cos60 * (left_eye_dst.x - right_eye_dst.x) -
      sin60 * (left_eye_dst.y - right_eye_dst.y) + right_eye_dst.x};
  static const float third_point_dst_y {sin60 * (left_eye_dst.x - right_eye_dst.x) +
      cos60 * (left_eye_dst.y - right_eye_dst.y) + right_eye_dst.y};
  static const cv::Point2f third_point_dst {third_point_dst_x, third_point_dst_y};

  // apply the transform and crop
  const cv::Point2f source_points[3] { left_eye_src, right_eye_src, third_point_src };
  static const cv::Point2f destination_points[3] = {left_eye_dst, right_eye_dst, third_point_dst};
  auto warp_mat = cv::getAffineTransform( source_points, destination_points );
  cv:: warpAffine(photo, photo, warp_mat,
		   cv::Size(properties::captured_image_width,properties::captured_image_height));
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
void helper::gl::display_cv_mat(const cv::Mat& mat) {
  if (mat.data) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);               
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, mat.ptr());
    glColor4f(1.f,1.f,1.f,1.f);
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

