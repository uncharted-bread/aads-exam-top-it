#ifndef DYNARRAY_HPP
#define DYNARRAY_HPP

#include <cstddef>

namespace gordejchik {
  template< class T >
  struct dynarray_t {
    T* data_;
    size_t size_;
    size_t capacity_;
  };

  template< class T >
  void init(dynarray_t< T >& arr)
  {
    arr.data_ = nullptr;
    arr.size_ = 0;
    arr.capacity_ = 0;
  }

  template< class T >
  void destroy(dynarray_t< T >& arr)
  {
    delete[] arr.data_;
    arr.data_ = nullptr;
    arr.size_ = 0;
    arr.capacity_ = 0;
  }

  template< class T >
  void pushBack(dynarray_t< T >& arr, const T& val)
  {
    if (arr.size_ == arr.capacity_) {
      size_t newCap = (arr.capacity_ == 0) ? 1 : arr.capacity_ * 2;
      T* newData = new T[newCap];
      try {
        for (size_t i = 0; i < arr.size_; ++i) {
          newData[i] = arr.data_[i];
        }
      } catch (...) {
        delete[] newData;
        throw;
      }
      delete[] arr.data_;
      arr.data_ = newData;
      arr.capacity_ = newCap;
    }
    arr.data_[arr.size_] = val;
    ++arr.size_;
  }

  template< class T >
  void removeAt(dynarray_t< T >& arr, size_t idx)
  {
    for (size_t i = idx + 1; i < arr.size_; ++i) {
      arr.data_[i - 1] = arr.data_[i];
    }
    --arr.size_;
  }

  template< class T >
  size_t size(const dynarray_t< T >& arr)
  {
    return arr.size_;
  }
}

#endif
