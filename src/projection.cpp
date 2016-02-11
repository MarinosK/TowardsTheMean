/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include "projection.h"

Projection::Projection() :
  projection_window {glfwCreateWindow(properties::projection_monitor_width,
  				      properties::projection_monitor_height, "Projection Window",
  				      properties::projection_monitor,NULL)},
  running {true},
  image_buffer {}, 
  window_width {},
  window_height {},
  images_to_fade_in {},
  fade_counter {0},
  fade_in_done {false},
  fade_out_done {false} {
    // load sample images to buffer
    std::vector<cv::Mat> samples {helper::loadSampleImages()};
    for (const auto& im : samples) image_buffer.add_image(im);
    // ---- set up GL
    if (!projection_window) throw std::runtime_error("Did not manages to create Capture Window");
    glfwSetKeyCallback(projection_window, helper::gl::key_callback);
    glfwMakeContextCurrent(projection_window);
    // glewExperimental=true; /// ****** ???
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW"); // init GLEW
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(properties::vsync);
    glfwGetFramebufferSize(projection_window, &window_width, &window_height);
  }

void Projection::gl_preample() {
  glfwMakeContextCurrent(projection_window);    
  glViewport(0, 0, window_width, window_height);
  glClearColor(BACKGROUND_COLOUR); 
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); 
}

void Projection::fade_in_new_images(cv::Mat mat) {
  if (!images_to_fade_in.empty()) { // if not empty
    if (fade_in_done && fade_out_done) { // if previous fades are done then start new fade_in
      fade_counter = glfwGetTime() + properties::new_image_fadein_time;
      fade_in_done = false;
    }
    if (!fade_in_done && fade_out_done) { // fading in
      float weight_a =  (fade_counter - glfwGetTime()) / properties::new_image_fadein_time;
      float weight_b =  1.f - weight_a;
      cv::addWeighted(mat,weight_a,images_to_fade_in.front(),weight_b, 0, mat);
      if (fade_counter >= glfwGetTime()) { // when done start a new fade_out
	fade_in_done = true;
	fade_out_done = false;
	fade_counter = glfwGetTime() + properties::new_image_fadein_time; // reset counter
      }
    }
    if (fade_in_done && !fade_out_done) { // fading out
      float weight_b =  (fade_counter - glfwGetTime()) / properties::new_image_fadein_time;
      float weight_a =  1.f - weight_b;
      cv::addWeighted(mat,weight_a,images_to_fade_in.front(),weight_b, 0, mat);
      if (fade_counter >= glfwGetTime()) { // when done start a new fade_out
	fade_out_done = true;
	image_buffer.add_image(images_to_fade_in.front()); // pop and add to the image_buffer
	images_to_fade_in.pop();
      }
    }
  }
}

void Projection::update_frame() {
  using namespace std::placeholders; 
  if (running) {
    if (glfwWindowShouldClose(projection_window)) // quit on escape
      throw helper::quit_program_exception();
    gl_preample();
    auto now = glfwGetTime();
    auto index = 0.f;
    cv::Mat average {image_buffer.binary_fold([&now, &index](auto im1, auto im2) {
	  cv::Mat result{};
	  auto weight = std::abs(sin(now / properties::animation_speed + (index+=1.3))) * 0.2 + 0.3;
	  cv::addWeighted(im1,weight,im2,weight,0,result);
	  return result;
	})};
    // maybe equalize somehow??
    helper::gl::display_cv_mat(average);
    fade_in_new_images(average);
    glfwSwapBuffers(projection_window);
    // glfwPollEvents();
  }
}
