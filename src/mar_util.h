
// Various C++11 Additions
// by Marinos Koutsomichalis (marinos@agxivatein.com)

/* this header is only meant for private use - if you ever happen to use it bare in mind that no guarantee whatsoever is given */

#ifndef MAR_UTIL_H
#define MAR_UTIL_H

#include <iosfwd>
#include <boost/type_index.hpp>

// concurency
#include <thread>
#include <exception>

// utility macros
#define DEBUG_PRINT(A) std::cout << #A << ": " << (A) << std::endl; 
#define PROJECT_INFO_PRINT(A) std::cout << "\n-----\n" << (A) << "\nDeveloped by Marinos Koutsomichalis (marinos@marinoskoutsomichalis.com)\nUse at your own risk!\n-----\n" << std::endl;
#define PRINT_TYPE_INFO(A) std::cout <<  #A << ": "<< boost::typeindex::type_id_with_cvr< decltype((A)) >().pretty_name() << std::endl;
#define GET_VAR_NAME(a) #a

namespace mar {

  // ============================= concurency ============================
  class scoped_thread{
  private:
    std::thread t;
  public:
    explicit scoped_thread(std::thread t_) : t{std::move(t_)} {
      if (!t.joinable()) throw std::logic_error("No Thread");
    }
    ~scoped_thread() { t.join();}
    scoped_thread(const scoped_thread&) =delete;
    scoped_thread& operator=(const scoped_thread&) =delete;
  };
  
}


#endif
