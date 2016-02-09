
#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

#include <memory>
#include <algorithm>
#include <mutex>
#include <vector>
#include <exception>
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
  template <typename Func> // excpect callable objects only
  inline void iterate(Func func) {
    std::lock_guard<std::mutex> lock(mtx);
    std::for_each(buffer.begin(),buffer.end(),func);
  }
  inline cv::Mat operator[](unsigned int index) const {
    if (index < buffer.size()) return buffer[index];
    else throw std::out_of_range("ImageBuffer: item's index is out of bounds");
  }
  inline unsigned int size() const {return buffer.size();}
  inline unsigned int capacity() const {return buffer.capacity();}
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

#endif


