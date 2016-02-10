
#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

#include <memory>
#include <algorithm>
#include <numeric>
#include <mutex>
#include <vector>
#include <exception>
#include <mar_algo.h>
#include <boost/circular_buffer.hpp>
#include <opencv2/core.hpp>
#include <boost/core/noncopyable.hpp>
#include "properties.h"

using im_buf = boost::circular_buffer<cv::Mat>;

class ImageBuffer : private boost::noncopyable { 
private:
  im_buf buffer;
  std::mutex mtx;
public:
  inline void add_image(const cv::Mat& mat) {
    std::lock_guard<std::mutex> lock(mtx);
    buffer.push_back(mat);
  }
  template <typename Func> // expect callable objects only
  inline void iterate(Func func, unsigned int index = 0) {
#ifndef UNSAFE_OPTIMISATIONS
    if (index > buffer.size())
      throw std::out_of_range("ImageBuffer: cannot iterate at this range, index out of bounds");
#endif // UNSAFE_OPTIMISATIONS
    std::lock_guard<std::mutex> lock(mtx);
    std::for_each(buffer.begin()+index,buffer.end(),func);
  }
  template <typename Func> // expect callable objects only
  inline cv::Mat binary_fold(Func func) { // func should be of cv::Mat (cv::Mat, cv::Mat) type
#ifndef UNSAFE_OPTIMISATIONS
    if (buffer.empty()) // faster than size() 
      throw std::out_of_range("ImageBuffer: cannot iterate at this range, index out of bounds");
#endif // UNSAFE_OPTIMISATIONS 
    std::lock_guard<std::mutex> lock(mtx);
    return mar::binary_fold(buffer.begin(), buffer.end(), func);
  }
  inline cv::Mat operator[](unsigned int index) {
#ifndef UNSAFE_OPTIMISATIONS
if (index < buffer.size()) {
#endif // UNSAFE_OPTIMISATIONS
      std::lock_guard<std::mutex> lock(mtx);
      return buffer[index];
#ifndef UNSAFE_OPTIMISATIONS
} else throw std::out_of_range("ImageBuffer: item's index is out of bounds");
#endif // UNSAFE_OPTIMISATIONS  
  }
  inline unsigned int size() {
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.size();
  }
  inline unsigned int capacity() {
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.capacity();
  }
  inline explicit ImageBuffer(const unsigned int capacity = properties::max_images_in_loop):
    buffer {capacity},
    mtx {} {}
  inline ImageBuffer(std::vector<cv::Mat> vec,
		     const unsigned int capacity = properties::max_images_in_loop):
    buffer {capacity},
    mtx {} { for (const auto& item : vec) add_image(item); }
  ~ImageBuffer()=default;
  ImageBuffer(const ImageBuffer&&)=delete; 
  ImageBuffer& operator=(const ImageBuffer&&)=delete;  
};

// NOT THREAD SAFE (FOR TESTING)
// class ImageBuffer : private boost::noncopyable { 
// private:
//   im_buf buffer;
// public:
//   inline void add_image(const cv::Mat& mat) { buffer.push_back(mat);}
//   template <typename Func> // excpect callable objects only
//   inline void iterate(Func func, unsigned int index = 0) const {
//     std::for_each(buffer.begin()+index,buffer.end(),func);
//   }
//   inline cv::Mat operator[](unsigned int index) const {return buffer[index];}
//   inline unsigned int size() const {return buffer.size();}
//   inline unsigned int capacity() const { return buffer.capacity();}
//   inline explicit ImageBuffer(const unsigned int capacity = properties::max_images_in_loop):
//     buffer {capacity} {}
//   inline ImageBuffer(std::vector<cv::Mat> vec,
// 		     const unsigned int capacity = properties::max_images_in_loop):
//     buffer {capacity} { for (const auto& item : vec) add_image(item); }
//   ~ImageBuffer()=default;
//   ImageBuffer(const ImageBuffer&&)=delete; 
//   ImageBuffer& operator=(const ImageBuffer&&)=delete;  
// };

#endif


