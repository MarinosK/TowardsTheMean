/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include "capture.h"

// implementation details
static constexpr unsigned int photos_wait_time4 {WAIT_TIME_BETWEEN_PHOTOS + 3};
static constexpr unsigned int photos_wait_time3 {WAIT_TIME_BETWEEN_PHOTOS + 2};
static constexpr unsigned int photos_wait_time2 {WAIT_TIME_BETWEEN_PHOTOS + 1};
static constexpr float photos_wait_time1 {WAIT_TIME_BETWEEN_PHOTOS + 0.5f};
static constexpr unsigned int photos_wait_time_still {WAIT_TIME_BETWEEN_PHOTOS};
static constexpr unsigned int photos_wait_time_done {WAIT_TIME_BETWEEN_PHOTOS - 1};

static std::string create_unique_folder(std::string root) {
  if (!(root.back() == '/')) root.push_back('/');
  auto day = boost::gregorian::day_clock::universal_day();
  std::ostringstream folder;
  unsigned int i{1};
  folder << root << day.month() << " " << day.day() << " " << day.year() << " session " << i;
  boost::filesystem::path path{folder.str()};
  while (boost::filesystem::is_directory(path)) {
    auto number_of_digits = static_cast<signed int>(std::log10(i)) + 1;
    folder.seekp(-number_of_digits,folder.cur);
    folder << i++;
    path = boost::filesystem::path {folder.str()}; 
  }
  if (!boost::filesystem::create_directories(path))
    throw std::runtime_error("Failed to create unique folder");
  return folder.str();
}

const std::string Capture::photo_folder_path_m {create_unique_folder(PHOTOS_PATH)};

Capture::Capture(Projection* proj) :
  capture_window_m { glfwCreateWindow(
    properties::capture_screen_width,properties::capture_screen_height,
    "Capture Window",properties::primary_monitor, NULL) },
  video_capture_m {},
  running_m {true},
  projection_process_m {proj},
  face_cascade_m {},
  eyes_cascade_m {},
  window_width_m {},
  window_height_m {},
  camera_width_m {},
  camera_height_m {},
  font_m {FONTS_PATH "Arial Bold.ttf"},
  capture_counter_m {0},
  photo_capture_flag_m {false},
  capture_done_flag_m {false},
  draw_frames_flag_m {true},
  face_out_of_range_msg_flag_m {false},
  photo_file_counter_m {1} {
    // ------ setup openGL
    if (!capture_window_m) throw std::runtime_error("Did not manage to create Capture Window");
    glfwSetKeyCallback(capture_window_m, helper::gl::key_callback);
    glfwMakeContextCurrent(capture_window_m);
    // glewExperimental=true; 
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW");
    glEnable(GL_TEXTURE_2D);
    glfwSwapInterval(properties::vsync);
    glfwGetFramebufferSize(capture_window_m, &window_width_m, &window_height_m);
    if (font_m.Error()) throw std::runtime_error("Failed to load font");
    // ------ set up CV
    if (!video_capture_m.open(properties::camera))
      throw std::runtime_error("Failed to initialise camera");
    video_capture_m.set(CV_CAP_PROP_FRAME_WIDTH,1920);
    video_capture_m.set(CV_CAP_PROP_FRAME_HEIGHT,1280);
    camera_width_m = video_capture_m.get(CV_CAP_PROP_FRAME_WIDTH);
    camera_height_m = video_capture_m.get(CV_CAP_PROP_FRAME_HEIGHT);
    HELPER_LOG_OUT("--camera resolution is: " << camera_width_m  << "x" << camera_height_m);
    if (!face_cascade_m.load(FACE_CASCADE))
      throw std::runtime_error("Failed to load face cascade classfier");
    if (!eyes_cascade_m.load(EYES_CASCADE))
      throw std::runtime_error("Failed to load eyes cascade classfier");
  }

void Capture::update_frame() {
  if (running_m) {
    if (glfwWindowShouldClose(capture_window_m)) // quit on escape
      throw helper::quit_program_exception();
    gl_preample();
    cv::Mat video_frame{};
    get_video_frame(video_frame);
    // zoom_in_video_frame(video_frame); // for the actual exhibition zooming has been necessary
    display_detect_capture_load_and_save_portrait(video_frame);
    glfwSwapBuffers(capture_window_m);
  }
}

void Capture::display_detect_capture_load_and_save_portrait(cv::Mat& video_frame) {
  static unsigned int face_detected_counter {0};
  static constexpr unsigned int maximum_limit_for_face_detected_counter {15};
  try {
    auto face = detect_face(video_frame, draw_frames_flag_m);
    if (face || face_detected_counter) { // detect face
      if (face) ++face_detected_counter; else --face_detected_counter;
      if (face_detected_counter > maximum_limit_for_face_detected_counter)
	face_detected_counter = maximum_limit_for_face_detected_counter;
      helper::gl::display_cv_mat(video_frame);
      face_out_of_range_msg_flag_m = false;
      if (!photo_capture_flag_m) { // capture
	photo_capture_flag_m = true;
	capture_counter_m = glfwGetTime() + 4 + WAIT_TIME_BETWEEN_PHOTOS; 
      } else if (glfwGetTime() < (capture_counter_m - photos_wait_time4)) {
	draw_frames_flag_m = true;
	render_text("Face detected! Look at the camera and stand still",250,window_height_m-176);
      } else if (glfwGetTime() <= (capture_counter_m - photos_wait_time3))
	render_text("3",window_width_m - 900,window_height_m/2,400);
      else if (glfwGetTime() <= (capture_counter_m - photos_wait_time2)) 
	render_text("2",window_width_m - 900,window_height_m/2,400);
      else if (glfwGetTime() <= (capture_counter_m - photos_wait_time1)) 
	render_text("1",window_width_m - 900,window_height_m/2,400);
      else if (glfwGetTime() <= (capture_counter_m - photos_wait_time_still)) {
	render_text("1",window_width_m - 900,window_height_m/2,400);
	draw_frames_flag_m = false; // don't draw frames the next few times so that the photo can be taken
      } else if (glfwGetTime() <= (capture_counter_m - photos_wait_time_done)) {
	render_text("done",2000,window_height_m/2,400);
	if (face) load_and_save_portait(video_frame, *face);
      } else if (glfwGetTime() < capture_counter_m - 1) 
	render_text("...processing photo..",2000,window_height_m/2);
      else { // reset
	photo_capture_flag_m = false;
	capture_done_flag_m = false;	
      }
    } else {
      helper::gl::display_cv_mat(video_frame);
      if (photo_capture_flag_m) photo_capture_flag_m = false;
      if (capture_done_flag_m) capture_done_flag_m = false;
    }}
  catch (helper::too_many_faces_exception& e) {
    if (photo_capture_flag_m) photo_capture_flag_m = false;
    helper::gl::display_cv_mat(video_frame);
    render_text("Too many faces! Please try one at a time",250,window_height_m-176);
  }
}

void Capture::load_and_save_portait(cv::Mat& video_frame, helper::opencv::Face& face) {
  if (!capture_done_flag_m) { // launch once only
    capture_done_flag_m = true; 
    draw_frames_flag_m = true;
    helper::opencv::allign_and_isolate_face(video_frame,face);
    cv::normalize(video_frame, video_frame, 0, 255, cv::NORM_MINMAX); // normalize
    std::ostringstream filename{};
    filename << photo_folder_path_m << "/photo " << photo_file_counter_m++ << ".tif"; 
    try {
      cv::imwrite(filename.str(),video_frame);
      HELPER_LOG_OUT( filename.str() << " succesfully saved");	  
    }
    catch (std::runtime_error& e) {
      HELPER_LOG_ERR( "failed to write " << filename.str() << " to disc with exception: " << e.what());	  
    }
    projection_process_m->add_to_the_animation(video_frame);
  }
}

boost::optional<helper::opencv::Face> Capture::detect_face(cv::Mat& frame, bool draw_frames){
  std::vector<cv::Rect> faces; // vector holding detected faces
  helper::opencv::Face face_object;
  cv::Mat frame_gray;
  cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
#ifndef UNSAFE_OPTIMISATIONS
  cv::equalizeHist(frame_gray, frame_gray); 
#endif // UNSAFE_OPTIMISATIONS
  face_cascade_m.detectMultiScale(frame_gray, faces, 1.1, 2, 0, cv::Size(150, 150), cv::Size(400, 400)); // detect faces
  if (faces.empty()) return boost::optional<helper::opencv::Face>{};
  for (auto& face : faces) { // else for each face detect eyes/mouth
    cv::Mat faceROI = frame_gray(face);
    std::vector<cv::Rect> eyes;
    eyes_cascade_m.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30), cv::Size(70, 70));
    if (eyes.size() == 2) {
      if (draw_frames) cv::rectangle(frame, face, FACE_FRAME_COLOUR, 2);
      for (auto& eye : eyes) {
    	eye = eye + cv::Point{face.x,face.y}; // make coordinates absolute
    	if (draw_frames) cv::rectangle(frame, eye, EYES_FRAME_COLOUR);
      }
      if (faces.size()==1) { // if only one face move the coordinates in the face_object
    	face_object.left_eye = std::move(eyes[0]);
    	face_object.right_eye = std::move(eyes[1]);
    	face_object.face = std::move(face);
    	return boost::optional<helper::opencv::Face>{face_object};
      } else throw helper::too_many_faces_exception(); // many faces
    } else return boost::optional<helper::opencv::Face>{};
  }
  return boost::optional<helper::opencv::Face>{}; // it should never reach here, this silences the compiler's warning
}
