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
  camera_height {} {
    // ------ setup  GL
    if (!capture_window) throw std::runtime_error("Did not manage to create Capture Window");
    glfwSetKeyCallback(capture_window, helper::gl::key_callback);
    glfwMakeContextCurrent(capture_window);
    // glewExperimental=true; 
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW");
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    // glEnable( GL_TEXTURE_2D );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(properties::vsync);
    glfwGetFramebufferSize(capture_window, &window_width, &window_height);
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
    helper::gl::display_cv_mat(video_frame);
  
    try {
      if (detect_face(video_frame)) { // detect face
	// increment counter
	// if counter => 
    	// instructions and > 3,2,1 (glfw)
	// if counter && if (auto face = detect_face(video_frame)) { // double check that all is OK
    	  // allign video_frame
    	  // proj->add
    	  // save_and_load_photo();
	// 
    	}
      } else {
    	// msg "Please stand in front of the camera to have your photo takens
      }
      // display video_frame
    }
    catch (helper::too_many_faces_exception& e) {
      // msg: too many faces detected etc
    }
    
    glfwSwapBuffers(capture_window);
    // glfwPollEvents();
  }
}

boost::optional<Face> Capture::detect_face(cv::Mat& frame){
  std::vector<cv::Rect> faces; // vector holding detected faces
  std::vector<cv::Rect> pair_of_eyes; // the LAST detected pair of eyes
  cv::Mat frame_gray;
  cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(frame_gray, frame_gray);
  face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, cv::Size(80, 80)); // detect faces
  if (faces.empty()) { // if not found then return
    cv::putText(frame, "Please sit opposite to the camera to have your photo portrait taken!",
		cv::Point(0,frame.rows), 3, 1, CAPTURE_MSG_COLOUR, 1, 8 );
    return boost::optional<Face>{};
  };
  // for (auto& face : faces) { // else for each face also detect eyes
  //   cv::Mat faceROI = frame_gray(face);
  //   std::vector<cv::Rect> eyes;
  //   eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE,
  // 				  cv::Size(30, 30)); 
  //   if( eyes.size() == 2) { 	// draw the frames around face/eyes
  //     cv::rectangle(frame, face, FACE_FRAME_COLOUR);
  //     for (auto& eye : eyes) {
  // 	cv::rectangle(frame, eye, EYES_FRAME_COLOUR);
  //     }
  //     pair_of_eyes = std::move(eyes); // so that this available later on
  //   }
  // }
  if (faces.size()>1) { // if many found the return, too
    cv::putText(frame, "Too many faces detected! Please try one at a time.",
		cv::Point(0,frame.rows), 3, 1, CAPTURE_MSG_COLOUR, 1, 8 );
    return boost::optional<Face>{};
  } else { // if exactly has been found
    cv::putText(frame, "Face Detected: please stand still facing the camera!",
		cv::Point(0,frame.rows), 3, 1, CAPTURE_MSG_COLOUR, 1, 8 );
    //    return Face{pair_of_eyes[0],pair_of_eyes[1],faces[0]};
    return boost::optional<Face>{};
    // throw exception
  }
}
