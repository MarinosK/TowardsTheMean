/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#include <iostream>
#include <string>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include "helper.h"

int main( int argc, char** argv) {
  try {
    PROJECT_INFO_PRINT ("Photo Average command line tool");
    std::string input_path;
    helper::parametrise_average(argc, argv, input_path);
    boost::filesystem::path input_folder {input_path};
    std::cout << input_path << std::endl;
    if (!boost::filesystem::is_directory(input_folder))
      std::cerr << input_path << " folder does not exist" << std::endl;  
    helper::bot::generate_average(input_path,helper::bot::generate_unique_filename_for_average());
    std::cout << "Goodbye!" << std::endl;
    return 0;
  }
  catch (const helper::param_help_exception& e) {
    return 0;
  }  catch (const cv::Exception& e) {
    std::cerr << e.what() << std::endl;
    return 1; 
  } catch (...) {
    std::cerr << "uknown exception" << std::endl;
    return 2; 
  }
}
  
