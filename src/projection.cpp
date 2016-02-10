#include "projection.h"

Projection::Projection(ImageBuffer& buffer) :
  projection_window {glfwCreateWindow(properties::projection_monitor_width,
  				      properties::projection_monitor_height, "Projection Window",
  				      properties::projection_monitor,NULL)},
  running {true},
  image_buffer {buffer},
  window_width {},
  window_height {} {
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

    // normalize average here?
      
    helper::gl::display_cv_mat(average);

    // average with opengl
    // image_buffer.iterate([](cv::Mat mat){
    // 	glPushMatrix();
    // 	helper::gl::display_cv_mat(mat,0.5);
    // 	glPopMatrix();
    //   }); // skip first element
    //
    // average with a container of Textures
    // ??
    
    glfwSwapBuffers(projection_window);
    // glfwPollEvents();
  }
}
