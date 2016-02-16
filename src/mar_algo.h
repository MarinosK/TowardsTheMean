// Various C++14 Additions
// by Marinos Koutsomichalis (marinos@agxivatein.com)

/* this header is only meant for private use - if you ever happen to use it bare in mind that no guarantee whatsoever is given */

#ifndef MAR_ALGO_H
#define MAR_ALGO_H

#include <algorithm>
#include <exception>
#include <iterator>
#include <cstddef>

namespace mar {
  
  /// folds a container by means of applying op to pairs recursively (associativeness is not standard)
  template<class It, class Func>
  auto binary_fold_rec(It begin, It end, Func op) ->  decltype(op(*begin, *end)) {
    std::ptrdiff_t diff = end - begin;
    switch (diff) {
      case 0: throw std::out_of_range("binary fold on empty container");
      case 1: return *begin;
      case 2: return op(*begin, *(begin + 1));
      default: { // first round to the nearest multiple of 2 and then advance
	It mid{begin};
	int div = diff/2;
	int offset = (div%2 == 1) ? (div+1) : div; // round to the closest multiple of two (upwards)
	std::advance(mid, offset);
	return op( binary_fold_rec(begin,mid,op), binary_fold_rec(mid,end,op) );
      }
    }
  }
  
}

#endif
