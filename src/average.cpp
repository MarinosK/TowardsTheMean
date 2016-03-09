/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include <iostream>
#include <string>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include "helper.h"
#include "mar_util.h"

int main( int argc, char** argv) {
  try {
    PROJECT_INFO_PRINT ("Photo Average command line tool");
    std::string input_path{};
    helper::parametrise_average(argc, argv, input_path);
    boost::filesystem::path input_folder {input_path};
    std::cout << input_path << "\n";
    if (!boost::filesystem::is_directory(input_folder))
      std::cerr << input_path << " folder does not exist\n";  
    helper::bot::generate_average(input_path,helper::bot::generate_unique_filename_for_average());
    std::cout << "Goodbye!\n";
    return 0;
  }
  catch (const helper::param_help_exception& e) {
    return 0;
  }  catch (const cv::Exception& e) {
    std::cerr << e.what() << "\n";
    return 1; 
  } catch (...) {
    std::cerr << "uknown exception\n";
    return 2; 
  }
}
  
