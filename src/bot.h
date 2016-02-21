/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

#ifndef BOT_H
#define BOT_H

#include <string>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace bot {

  void generate_average(std::string path, std::string output_file);
  
}



#endif /* BOT_H */
