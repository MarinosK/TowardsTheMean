/*
  Coded by Marinos Koutsomichalis for the 'Towards The Mean' photo installation project.
  Towards The Mean (c) 2016 Marianne Holm Hansen. 
*/

// I don't need a thread-safe buffer eventually!!

#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

#include <algorithm>
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
public:
  inline void add_image(const cv::Mat& mat) {buffer.push_back(mat);}
  template <typename Func> // expect callable objects only
  inline void iterate(Func func, unsigned int index = 0) {
#ifndef UNSAFE_OPTIMISATIONS
    if (index > buffer.size())
      throw std::out_of_range("ImageBuffer: cannot iterate at this range, index out of bounds");
#endif // UNSAFE_OPTIMISATIONS
    std::for_each(buffer.begin()+index,buffer.end(),func);
  }
  template <typename Func> // expect callable objects only
  inline cv::Mat binary_fold(Func func) { // func should be of cv::Mat (cv::Mat, cv::Mat) type
#ifndef UNSAFE_OPTIMISATIONS
    if (buffer.empty()) // faster than size() 
      throw std::out_of_range("ImageBuffer: cannot iterate at this range, index out of bounds");
#endif // UNSAFE_OPTIMISATIONS 
    return mar::binary_fold_rec(buffer.begin(), buffer.end(), func);
  }
  inline cv::Mat operator[](unsigned int index) {
#ifndef UNSAFE_OPTIMISATIONS
    if (index < buffer.size()) {
#endif // UNSAFE_OPTIMISATIONS
      return buffer[index];
#ifndef UNSAFE_OPTIMISATIONS
    } else throw std::out_of_range("ImageBuffer: item's index is out of bounds");
#endif // UNSAFE_OPTIMISATIONS  
  }
  inline unsigned int size() { return buffer.size();}
  inline unsigned int capacity() {return buffer.capacity();}
  inline explicit ImageBuffer(const unsigned int capacity = properties::max_images_in_loop):
    buffer {capacity} {}
  inline ImageBuffer(std::vector<cv::Mat> vec,
  		     const unsigned int capacity = properties::max_images_in_loop):
    buffer {capacity} { for (const auto& item : vec) add_image(item); }
  ~ImageBuffer()=default;
  ImageBuffer(const ImageBuffer&&)=delete; 
  ImageBuffer& operator=(const ImageBuffer&&)=delete;  
};


#endif


