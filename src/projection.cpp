/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include "projection.h"

Projection::Projection() :
  projection_window_m {glfwCreateWindow(
    properties::projection_monitor_width,properties::projection_monitor_height,
    "Projection Window",properties::projection_monitor,NULL)},
  running_m {true},
  image_buffer_m {}, 
  window_width_m {},
  window_height_m {},
  images_to_fade_in_m {},
  fade_counter_m {0},
  fade_in_done_m {true},
  fade_out_done_m {true} {
    // load sample images to buffer
    std::vector<cv::Mat> samples {helper::loadSampleImages()};
    for (const auto& im : samples) image_buffer_m.add_image(im);
    // ---- set up GL
    if (!projection_window_m) throw std::runtime_error("Did not manages to create Capture Window");
    glfwSetKeyCallback(projection_window_m, helper::gl::key_callback);
    glfwMakeContextCurrent(projection_window_m);
    // glewExperimental=true; 
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW"); // init GLEW
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(properties::vsync);
    glfwGetFramebufferSize(projection_window_m, &window_width_m, &window_height_m);
  }

void Projection::fade_in_new_images(cv::Mat& mat) {
  if (!images_to_fade_in_m.empty()) { // if not empty
    if (fade_in_done_m && fade_out_done_m) { // if previous fades are done then start new fade_in
      fade_counter_m = glfwGetTime() + properties::new_image_fadein_time;
      fade_in_done_m = false;
    }
    if (!fade_in_done_m && fade_out_done_m) { // fading in
      float weight_a = (fade_counter_m - glfwGetTime()) / properties::new_image_fadein_time; 
      float weight_b =  1.f - weight_a;
      cv::addWeighted(mat,weight_a,images_to_fade_in_m.front(),weight_b, 0, mat);
      if (fade_counter_m <= glfwGetTime()) { // when done start a new fade_out
	fade_in_done_m = true;
	fade_out_done_m = false;
	image_buffer_m.add_image(images_to_fade_in_m.front()); // add to the image_buffer here (so that shifts in the order are not noticed)
	fade_counter_m = glfwGetTime() + properties::new_image_fadein_time; // reset counter
      }
    }
    if (fade_in_done_m && !fade_out_done_m) { // fading out
      float weight_b =  (fade_counter_m - glfwGetTime()) / properties::new_image_fadein_time;
      float weight_a =  1.f - weight_b;
      cv::addWeighted(mat,weight_a,images_to_fade_in_m.front(),weight_b, 0, mat);
      if (fade_counter_m <= glfwGetTime()) { 
	fade_out_done_m = true;
	images_to_fade_in_m.pop(); 
      }
    }
  }
}

void Projection::update_frame() {
  using namespace std::placeholders;
  if (running_m) {
    if (glfwWindowShouldClose(projection_window_m)) // quit on escape
      throw helper::quit_program_exception();
    gl_preample();
    glTranslatef(0.12,0,0); // translate to compensate for a problem with the projector
    auto now = glfwGetTime();
    auto index = 0.f;
    cv::Mat average {image_buffer_m.binary_fold([&now, &index](auto im1, auto im2) {
	  cv::Mat result{};
	  auto weight = std::abs(sin(now / properties::animation_speed + (index+=1.3))) * 0.2 + 0.3;
	  cv::addWeighted(im1,weight,im2,weight,0,result);
	  return result;
	})};
    fade_in_new_images(average);
    // cv::normalize(average, average, 0, 255, cv::NORM_MINMAX);
    constexpr unsigned int border_size {300};
    // cv::copyMakeBorder(average, average, 0, 0, border_size, border_size, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
    cv::Mat left_border_roi {average(cv::Rect(0,0,border_size,average.rows))};
    cv::Mat right_border_roi {average(cv::Rect(average.cols-border_size,0,border_size,average.rows))};
    left_border_roi.setTo(0);
    right_border_roi.setTo(0);
    helper::gl::display_cv_mat(average);
    glfwSwapBuffers(projection_window_m);
  }
}
