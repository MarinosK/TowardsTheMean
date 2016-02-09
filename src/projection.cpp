#include "projection.h"

Projection::Projection(ImageBuffer& buffer) :
  projection_window {glfwCreateWindow(properties::projection_monitor_width,
  				      properties::projection_monitor_height, "Projection Window",
  				      properties::projection_monitor,NULL)},
  running {true},
  image_buffer {buffer},
  width {},
  height {},
  ratio {} {
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
    glfwGetFramebufferSize(projection_window, &width, &height);
    ratio = static_cast<float>(width) / static_cast<float>(height);
#ifdef DEBUG   
    HELPER_LOG_OUT( "--projection monitor FrameBuffer's resolution set to: " << width << "x" << height);
#endif // DEBUG
  }

void Projection::gl_preample() {
  glfwMakeContextCurrent(projection_window);    
  glViewport(0, 0, width, height);
  glClearColor(BACKGROUND_COLOUR); 
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); 
}

void Projection::update_frame() {
  if (running) {
    if (glfwWindowShouldClose(projection_window)) // quit on escape
      throw helper::quit_program_exception();
    gl_preample();

    cv::Mat average{};
    image_buffer.iterate([&average](cv::Mat mat){
	cv::addWeighted(average,0.5,mat,0.5,0,average);
      });
    // properties::animation::speed
    // glfwGetTime();
    helper::gl::display_cv_mat(average);
     
    glfwSwapBuffers(projection_window);
  }
}
