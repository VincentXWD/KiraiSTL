#ifndef KIRAI_LIB_VECTOR_H
#define KIRAI_LIB_VECTOR_H

#include <cstddef>

#include "allocator.h"

namespace kirai {

template<typename T, typename Alloc = alloc>
class vector {
 public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef value_type* iterator;
  typedef value_type& reference;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;

 protected:
  iterator allocate_and_fill(size_type n, const T& val) {
    iterator result = data_allocator::allocate(n);
    uninitialized_fill_n(result, n, val);
    return result;
  }

  void fill_initialize(size_type n, const T& val) {
    start = allocate_and_fill(n, val);
    finish = start + n;
    end_of_storage = finish;
  }

  void deallocate() {
    if (start) {
      data_allocator::deallocate(start, end_of_storage - start);
    }
  }
  void insert_aux(iterator position, const T& value);

 protected:
  typedef simple_alloc<value_type, Alloc> data_allocator;

  iterator start;
  iterator finish;
  iterator end_of_storage;

 public:

  iterator begin() {
    return start;
  }

  iterator end() {
    return finish;
  }

  size_type size() {
    return end() - begin();
  }

  size_type max_size() {
    return end_of_storage - begin();
  }

  size_type capacity() {
    return end_of_storage - begin();
  }
  bool empty() {
    return begin() == end();
  }

  reference operator [](size_type n) {
    return *(begin() + n);
  }

  reference front() {
    return *begin();
  }

  reference back() {
    return *(end() - 1);
  }
  reference at(size_type n) {
    return *(begin() + n);
  }

  void push_back(const T& val);

  void pop_back();

  iterator insert (iterator position, const T& val);

  iterator insert (iterator position, size_type n, const T& val); 

  iterator erase(iterator position) {
    if (position != end()) {
      copy(position + 1, finish, position);
    }
    --finish;
    destroy(finish);
    return position;
  }

  iterator erase (iterator first, iterator last) {
    iterator i = copy(last, finish, first);
    destroy(i, finish);
    finish = finish - (last - first);
    return first;
  }

  void clear() {
    destroy(begin(), end());
    start = 0;
    finish = 0;
    end_of_storage = 0;
  }

  void swap(vector& x) {
    iterator temp_start = start;
    iterator temp_finish = finish;
    iterator temp_end_of_storage = end_of_storage;

    start = x.begin();
    finish = x.end();
    end_of_storage = x.end_of_storage;

    x.start = temp_start;
    x.finish = temp_finish;
    x.end_of_storage = temp_end_of_storage;
  }

  void resize(size_type n,const T& val) {
    if (n < size()) erase(begin() + n, end());
    else insert(end(), n - size(), val);
  }

 public:
  vector() : start(0), finish(0), end_of_storage(0) {
  }

  vector(size_type n, const T& val) {
    fill_initialize(n, val);
  }

  vector(int n, const T& val) {
    fill_initialize(n, val);
  }

  vector(long n, const T& val) {
    fill_initialize(n, val);
  }

  explicit vector(size_type n) {
    fill_initialize(n, value_type());
  }

  ~vector() {
    destroy(start, finish);
    deallocate();
  }

};

} // namespace kirai

#endif
