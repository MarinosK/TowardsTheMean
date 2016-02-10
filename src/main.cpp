/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include <vector>
#include <chrono>
#include <mar_util.h>
#include <thread>
#include <atomic>
#include <opencv2/core.hpp>
#include "properties.h"
#include "helper.h"
#include "capture.h"
#include "imageBuffer.h"
#include "projection.h"

int main( int argc, char** argv) {
  // aliases
  using clock = std::chrono::steady_clock;
  using minutes = std::chrono::minutes;
  using namespace std::literals;
  try {
    PROJECT_INFO_PRINT 
      ("Towards The Mean (photo installation by Marianne Holm Hansen)");
    // --------------------- setup -----------------------
    clock::time_point program_start_time {clock::now()};
    helper::logging::setup();
    helper::parametrise(argc, argv);
    helper::gl::setup();
    std::vector<cv::Mat> sampleImages {helper::loadSampleImages()}; 
    ImageBuffer images{sampleImages, properties::max_images_in_loop}; // thread-safe buffer
    Capture cap {images}; // capture interface
    Projection proj {images}; // projection interface
    std::atomic<bool> program_should_quit {false};
    // --------------- quit after the user-defined number of minutes ------------------
    std::thread quit_after_minutes {[&]{ while (true) {
	  clock::time_point time_flag_now {clock::now()};
	  auto elapsed_minutes_from_launch = std::chrono::duration_cast<minutes>
	    (time_flag_now - program_start_time);
	  if (elapsed_minutes_from_launch.count() >= properties::quit_after_minutes)
	    program_should_quit = true;
	  std::this_thread::sleep_for(30s); // we don't need better accuracy really..
	}}};
    quit_after_minutes.detach();
    // --------------- quit if 'q' is pressed  ------------------
    std::thread quit_on_q {[&]{ while (true) {
	  if ( std::cin.get() == 'q' ) program_should_quit = true;
	}}};
    quit_on_q.detach();
    // --------------------- Main loop -----------------------
    while (!program_should_quit) {
      cap.update_frame(); // will spawn threads when a new photo is taken
      proj.update_frame(); // animation loop
      glfwPollEvents();
    }
    throw helper::quit_program_exception();
    return 0; // it should never reach here normally
  }
  // ----------------------  exception handling -----------------
  catch (const helper::param_help_exception& e) { return 0; }
  catch (const helper::quit_program_exception& e) {
    // HELPER_LOG_OUT("creating daily average..");
    // HELPER_LOG_OUT("tweeting daily average..");
    // HELPER_LOG_OUT("emailing daily average..");
    // Bot bot {image_folder};
    // bot.export_session_average();
    // bot.tweet_session_average(); 
    // bot.email_session_average();
    HELPER_LOG_OUT("cleaning up..."); 
    helper::gl::clean();
    HELPER_LOG_OUT("Goodbye!!");
    return 0;
  }
  catch (const std::runtime_error& e) {
    HELPER_LOG_ERR(e.what());
#ifndef DEBUG 
    helper::gl::clean();
    return 1; // in release mode don't terminate even if errors occur !!
#endif // DEBUG    
  }
  catch (const cv::Exception& e) {
    HELPER_LOG_ERR(e.what());
#ifndef DEBUG 
    helper::gl::clean();
    return 2; // in release mode don't terminate even if errors occur !!
#endif // DEBUG   
  }
  catch (const std::exception& e) {
    HELPER_LOG_ERR(e.what());
#ifndef DEBUG 
    helper::gl::clean();
    return 3; // in release mode don't terminate even if errors occur !!
#endif // DEBUG   
  }
  catch (...) {
    HELPER_LOG_ERR("uknown exception");
#ifndef DEBUG 
    helper::gl::clean();
    return 4; // in release mode don't terminate even if errors occur !!
#endif // DEBUG   
  }
}

