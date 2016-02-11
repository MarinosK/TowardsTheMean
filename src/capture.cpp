/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include "capture.h"

Capture::Capture(Projection* proj) :
  capture_window { glfwCreateWindow(
    properties::capture_screen_width,properties::capture_screen_height,
    "Capture Window",properties::primary_monitor, NULL) },
  video_capture {},
  running {true},
  projection_process {proj},
  face_cascade {},
  eyes_cascade {},
  window_width {},
  window_height {},
  camera_width {},
  camera_height {},
  font {FONTS_PATH "Palatino Linotype.ttf"},
  capture_counter {0},
  photo_capture_flag {false} {
    // ------ setup openGL
    if (!capture_window) throw std::runtime_error("Did not manage to create Capture Window");
    glfwSetKeyCallback(capture_window, helper::gl::key_callback);
    glfwMakeContextCurrent(capture_window);
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW");
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(properties::vsync);
    glfwGetFramebufferSize(capture_window, &window_width, &window_height);
    if (font.Error()) throw std::runtime_error("Failed to load font");
    // ------ set up CV
    if (!video_capture.open(properties::camera))
      throw std::runtime_error("Failed to initialise camera");
    camera_width = video_capture.get(CV_CAP_PROP_FRAME_WIDTH);
    camera_height = video_capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    HELPER_LOG_OUT("--camera resolution is: " << camera_width  << "x" << camera_height);
    if (!face_cascade.load(FACE_CASCADE))
      throw std::runtime_error("Failed to load face cascade classfier");
    if (!eyes_cascade.load(EYES_CASCADE))
      throw std::runtime_error("Failed to load face cascade classfier");
  }

void Capture::gl_preample() {
  glfwMakeContextCurrent(capture_window);    
  glViewport(0, 0, window_width, window_height);
  glClearColor(BACKGROUND_COLOUR); 
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); 
}

void Capture::update_frame() {
  if (running) {
    if (glfwWindowShouldClose(capture_window)) // quit on escape
      throw helper::quit_program_exception();
    gl_preample();
    cv::Mat video_frame{};
    get_video_frame(video_frame);
    display_detect_capture_load_and_save_portrait(video_frame);
    glfwSwapBuffers(capture_window);
    // glfwPollEvents();
  }
}

void Capture::display_detect_capture_load_and_save_portrait(cv::Mat& video_frame) {
   try { 
      if (detect_face(video_frame)) { // detect face
	helper::gl::display_cv_mat(video_frame);
	if (!photo_capture_flag) { // capture
	  capture_counter = glfwGetTime() + 15; // includes 10 seconds dead time in-beween
	  photo_capture_flag = true;
	} else if (glfwGetTime() < (capture_counter-13)) 
	  render_text("Face detected! Please allign to the center",10,window_height-76);
	else if (glfwGetTime() < (capture_counter-12)) 
	  render_text("3",window_width/2,window_height/2,200);
	else if (glfwGetTime() < (capture_counter-11)) {
	  render_text("2",window_width/2,window_height/2,200);
	} else if (glfwGetTime() < (capture_counter-10))
	  render_text("1",window_width/2,window_height/2,200);
	else if (glfwGetTime() < (capture_counter-9))
	  render_text("done",window_width/2,window_height/2,200);
	else if (glfwGetTime() < (capture_counter-1)) {
	  render_text("...processing image..",(window_width/2)-50,window_height/2);
	 // if counter && if (auto face = detect_face(video_frame)) { // double check that all is OK
    	  // allign video_frame
    	  // proj->add_to_animation
	// reset counter
    	  // save_to_disc;
	// 
	} else photo_capture_flag = false; // reset
      } else {
	helper::gl::display_cv_mat(video_frame);
	if (photo_capture_flag) photo_capture_flag = false;
	render_text("Position yourself opposite to the screen to have your picture taken",
		    10,window_height-76);
      }
    }
    catch (helper::too_many_faces_exception& e) {
      if (photo_capture_flag) photo_capture_flag = false;
      helper::gl::display_cv_mat(video_frame);
      render_text("Too many faces! Please try one at a time",10,10);
    }
}

void Capture::render_text(const char* text, double x, double y, int fontSize) {
  font.FaceSize(fontSize);
  font.Render(text, -1, FTPoint(x,y));
}

boost::optional<Face> Capture::detect_face(cv::Mat& frame){
  std::vector<cv::Rect> faces; // vector holding detected faces
  cv::Mat frame_gray;
  Face faceObject;
  cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(frame_gray, frame_gray); // **** maybe leave out for optimisation
  face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, cv::Size(80, 80)); // detect faces
  if (faces.empty()) return boost::optional<Face>{}; // if not found return a non-Face
  for (auto& face : faces) { // else for each face detect eyes
    cv::Mat faceROI = frame_gray(face);
    std::vector<cv::Rect> eyes;
    eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE,cv::Size(30, 30)); 
    if( eyes.size() == 2) {  // draw the frames around face/eyes
      cv::rectangle(frame, face, FACE_FRAME_COLOUR);
      for (auto& eye : eyes) {
	eye = eye + cv::Point{face.x,face.y}; // make coordinates absolute
	cv::rectangle(frame, eye, EYES_FRAME_COLOUR);
      }
      faceObject = {eyes[0],eyes[1],face}; // we won't have more that a couple of faces anyway so it's ok
    }
  }
  if (faces.size()>1) throw helper::too_many_faces_exception(); // many faces
  else return faceObject;
}
