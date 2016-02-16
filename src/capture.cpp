/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include "capture.h"

// implementation details
static constexpr unsigned int photos_wait_time4 {WAIT_TIME_BETWEEN_PHOTOS + 3};
static constexpr unsigned int photos_wait_time3 {WAIT_TIME_BETWEEN_PHOTOS + 2};
static constexpr unsigned int photos_wait_time2 {WAIT_TIME_BETWEEN_PHOTOS + 1};
static constexpr unsigned int photos_wait_time1 {WAIT_TIME_BETWEEN_PHOTOS};
static constexpr double photos_wait_time_almost_done {WAIT_TIME_BETWEEN_PHOTOS - 0.5};
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
  font_m {FONTS_PATH "Palatino Linotype.ttf"},
  capture_counter_m {0},
  photo_capture_flag_m {false},
  capture_done_flag_m {false},
  draw_frames_flag_m {true},
  face_out_of_range_msg_flag_m {false},
  photo_folder_path_m {create_unique_folder(PHOTOS_PATH)},
  photo_file_counter_m {1} {
    // ------ setup openGL
    if (!capture_window_m) throw std::runtime_error("Did not manage to create Capture Window");
    glfwSetKeyCallback(capture_window_m, helper::gl::key_callback);
    glfwMakeContextCurrent(capture_window_m);
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW");
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(properties::vsync);
    glfwGetFramebufferSize(capture_window_m, &window_width_m, &window_height_m);
    if (font_m.Error()) throw std::runtime_error("Failed to load font");
    // ------ set up CV
    if (!video_capture_m.open(properties::camera))
      throw std::runtime_error("Failed to initialise camera");
    camera_width_m = video_capture_m.get(CV_CAP_PROP_FRAME_WIDTH);
    camera_height_m = video_capture_m.get(CV_CAP_PROP_FRAME_HEIGHT);
    HELPER_LOG_OUT("--camera resolution is: " << camera_width_m  << "x" << camera_height_m);
    if (!face_cascade_m.load(FACE_CASCADE))
      throw std::runtime_error("Failed to load face cascade classfier");
    if (!eyes_cascade_m.load(EYES_CASCADE))
      throw std::runtime_error("Failed to load face cascade classfier");
  }

void Capture::gl_preample() {
  glfwMakeContextCurrent(capture_window_m);    
  glViewport(0, 0, window_width_m, window_height_m);
  glClearColor(BACKGROUND_COLOUR); 
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); 
}

void Capture::update_frame() {
  if (running_m) {
    if (glfwWindowShouldClose(capture_window_m)) // quit on escape
      throw helper::quit_program_exception();
    gl_preample();
    cv::Mat video_frame{};
    get_video_frame(video_frame);
    display_detect_capture_load_and_save_portrait(video_frame);
    glfwSwapBuffers(capture_window_m);
    // glfwPollEvents();
  }
}

void Capture::display_detect_capture_load_and_save_portrait(cv::Mat& video_frame) {
  try {
    if (auto face = detect_face(video_frame, draw_frames_flag_m)) { // detect face
      helper::gl::display_cv_mat(video_frame);
      face_out_of_range_msg_flag_m = false;
      if (!photo_capture_flag_m) { // capture
	photo_capture_flag_m = true;
	capture_counter_m = glfwGetTime() + 5 + WAIT_TIME_BETWEEN_PHOTOS; 
      } else if (glfwGetTime() < (capture_counter_m - photos_wait_time4))
	render_text("Face detected! Look at the camera and stand still",10,window_height_m-76);
      else if (glfwGetTime() <= (capture_counter_m - photos_wait_time3))
	render_text("3",window_width_m/2,window_height_m/2,200);
      else if (glfwGetTime() <= (capture_counter_m - photos_wait_time2)) 
	render_text("2",window_width_m/2,window_height_m/2,200);
      else if (glfwGetTime() <= (capture_counter_m - photos_wait_time1)) 
	render_text("1",window_width_m/2,window_height_m/2,200);
      else if (glfwGetTime() <= (capture_counter_m - photos_wait_time_almost_done)) {
	render_text("done",window_width_m/2,window_height_m/2,200);
	draw_frames_flag_m = false; // don't draw frames the next few times so that the photo can be taken
      } else if (glfwGetTime() <= (capture_counter_m - photos_wait_time_done)) {
	render_text("done",window_width_m/2,window_height_m/2,200);
	load_and_save_portait(video_frame, *face);
      } else if (glfwGetTime() < capture_counter_m - 1) 
	render_text("...processing photo..",(window_width_m/2)-50,window_height_m/2);
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
    render_text("Too many faces! Please try one at a time",10,10);
  }
}

void Capture::load_and_save_portait(cv::Mat& video_frame, helper::opencv::Face& face) {
  if (!capture_done_flag_m) { // launch once only
    capture_done_flag_m = true; // thread is launched
    draw_frames_flag_m = true; // draw frames again next time
    // cv::Mat photo {video_frame.clone()}; // so that we don't have any clashes with the camera
    helper::opencv::allign_and_isolate_face(video_frame,face);
    std::ostringstream filename{};
    filename << photo_folder_path_m << "/photo " << photo_file_counter_m++ << ".tif"; 
    try {
      cv::imwrite(filename.str(),video_frame);
      HELPER_LOG_OUT( filename.str() << " succesfully saved");	  
    }
    catch (std::runtime_error& e) {
      HELPER_LOG_ERR( "failed to write " << filename.str()
		      << " to disc with exception: " << e.what());	  
    }
    // helper::opencv::pad_and_resize_photo(photo);
    // projection_process->add_to_the_animation(photo);
  }
}

void Capture::render_text(const char* text, double x, double y, int fontSize) {
  font_m.FaceSize(fontSize);
  font_m.Render(text, -1, FTPoint(x,y));
}

boost::optional<helper::opencv::Face> Capture::detect_face(cv::Mat& frame, bool draw_frames){
  std::vector<cv::Rect> faces; // vector holding detected faces
  cv::Mat frame_gray;
  helper::opencv::Face faceObject;
  cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(frame_gray, frame_gray); // **** maybe leave out for optimisation
  face_cascade_m.detectMultiScale(frame_gray, faces, 1.1, 2, 0, cv::Size(80, 80)); // detect faces
  if (faces.empty()) return boost::optional<helper::opencv::Face>{};
  for (auto& face : faces) { // else for each face detect eyes
    cv::Mat faceROI = frame_gray(face);
    std::vector<cv::Rect> eyes;
    eyes_cascade_m.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE,cv::Size(30, 30)); 
    if( eyes.size() == 2) {
      if (draw_frames) cv::rectangle(frame, face, FACE_FRAME_COLOUR);
      for (auto& eye : eyes) {
	eye = eye + cv::Point{face.x,face.y}; // make coordinates absolute
	if (draw_frames) cv::rectangle(frame, eye, EYES_FRAME_COLOUR);
      }
      // * not optimal to move for each face just to keep the last, but there won't be many faces anyway
      faceObject.left_eye = std::move(eyes[0]);
      faceObject.right_eye = std::move(eyes[1]);
      faceObject.face = std::move(face);
    } else return boost::optional<helper::opencv::Face>{}; // not considered detected unless eyes have been found!!
  }
  if (faces.size()>1) throw helper::too_many_faces_exception(); // many faces
  else return boost::optional<helper::opencv::Face>{faceObject};
}

