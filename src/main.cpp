/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include <chrono>
#include <thread>
#include <atomic>
#include <sstream>
#include <opencv2/core.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include "properties.h"
#include "helper.h"
#include "capture.h"
#include "projection.h"
#include "mar_util.h"

int main( int argc, char** argv) {
  using clock = std::chrono::steady_clock;
  using minutes = std::chrono::minutes;
  using namespace std::literals;
  try {
    PROJECT_INFO_PRINT ("Towards The Mean (photo installation by Marianne Holm Hansen)");
    // --------------------- setup -----------------------
    clock::time_point program_start_time {clock::now()};
    helper::logging::setup();
    helper::parametrise(argc, argv);
    helper::gl::setup();
    Projection proj {};  // animation window
    Capture cap {&proj}; // capture, allign, save to disk and pass to the projection process
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
      proj.update_frame();
#ifdef UNSAFE_OPTIMISATIONS // half frame rate - should not be needed
      glfwPollEvents();
#endif
      cap.update_frame();
      glfwPollEvents(); 
    }
    throw helper::quit_program_exception();
    return 0; // it should never reach here normally
  }
  // ----------------------  exception handling -----------------
  catch (const helper::param_help_exception& e) {
    return 0;
  } catch (const helper::quit_program_exception& e) { // a normal session should end up here
    HELPER_LOG_OUT("generating session average..");
    if (boost::filesystem::is_directory(SESSION_AVERAGE_PATH))
      if (boost::filesystem::create_directories(SESSION_AVERAGE_PATH)) 
	throw std::runtime_error("Failed to create session average folder");
    std::ostringstream filename;
    filename << SESSION_AVERAGE_PATH << helper::bot::generate_unique_filename_for_average();
    helper::bot::generate_average(Capture::get_photo_folder_path(),filename.str());
    // HELPER_LOG_OUT("tweeting session average..");
    // to implement
    // HELPER_LOG_OUT("emailing session average..");
    // helper::bot::send_email(filename.str()); // to implement
    HELPER_LOG_OUT("cleaning up..."); 
    helper::gl::clean();
    HELPER_LOG_OUT("Goodbye!!");
    return 0;
  } catch (const std::runtime_error& e) {
    HELPER_LOG_ERR(e.what());
#ifndef DEBUG 
    helper::gl::clean();
    return 1; // in release mode don't terminate even if errors occur !!
#endif // DEBUG    
  } catch (const cv::Exception& e) {
    HELPER_LOG_ERR(e.what());
#ifndef DEBUG 
    helper::gl::clean();
    return 2; // in release mode don't terminate even if errors occur !!
#endif // DEBUG   
  } catch (const std::exception& e) {
    HELPER_LOG_ERR(e.what());
#ifndef DEBUG 
    helper::gl::clean();
    return 3; // in release mode don't terminate even if errors occur !!
#endif // DEBUG   
  } catch (...) {
    HELPER_LOG_ERR("uknown exception");
#ifndef DEBUG 
    helper::gl::clean();
    return 4; // in release mode don't terminate even if errors occur !!
#endif // DEBUG   
  }
}

