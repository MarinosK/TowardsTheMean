
// Various C++11 Additions
// by Marinos Koutsomichalis (marinos@agxivatein.com)

/* this header is only meant for private use - if you ever happen to use it bare in mind that no guarantee whatsoever is given */

#ifndef MAR_UTIL_H
#define MAR_UTIL_H

#include <iosfwd>
#include <boost/type_index.hpp>


// utility macros
#define DEBUG_PRINT(A) std::cout << #A << ": " << (A) << std::endl; 
#define PROJECT_INFO_PRINT(A) std::cout << "\n-----\n" << (A) << "\nDeveloped by Marinos Koutsomichalis (marinos@marinoskoutsomichalis.com)\nUse at your own risk!\n-----\n" << std::endl;
#define PRINT_TYPE_INFO(A) std::cout <<  #A << ": "<< boost::typeindex::type_id_with_cvr< decltype((A)) >().pretty_name() << std::endl;
#define GET_VAR_NAME(a) #a

// namespace mar {

// }


#endif
