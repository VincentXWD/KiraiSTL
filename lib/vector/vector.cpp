#include "vector.h"

namespace kirai {

template <typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T &value) {
  if (finish != end_of_storage) {
    construct(finish, *(finish - 1));
    ++finish;
    copy_backward(position, finish - 2, finish - 1);
    *position = value;
  } else {
    const size_type old_size = size();
    const size_type new_size = (old_size == 0 ? 1 : 2 * old_size);

    iterator new_start = data_allocator::allocate(new_size);
    iterator new_finish = uninitialized_copy(start, position, new_start);

    construct(new_finish, value);
    ++new_finish;
    uninitialized_copy(position, finish, new_finish);

    destroy(begin(), end());
    deallocate();
    start = new_start;
    finish = new_finish;
    end_of_storage = start + new_size;
  }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::push_back(const T &val) {
  if (finish != end_of_storage) {
    construct(finish, val);
    ++finish;
  } else {
    insert_aux(end(), val);
  }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::pop_back() {
  --finish;
  destroy(finish);
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, const T &val) {
  if (finish != end_of_storage) {
    construct(finish, *(finish - 1));
    ++finish;
    copy_backward(position, finish - 2, finish - 1);
    *position = val;
    return position;
  } else {
    difference_type offset = position - start;

    const size_type old_size = size();
    const size_type new_size = (old_size == 0 ? 1 : 2 * old_size);

    iterator new_start = data_allocator::allocate(new_size);
    iterator new_finish = uninitialized_copy(start, position, new_start);

    construct(new_finish, val);
    ++new_finish;
    uninitialized_copy(position, finish, new_finish);

    destroy(begin(), end());
    deallocate();

    start = new_start;
    finish = new_finish;
    end_of_storage = start + new_size;

    return (start + offset);
  }
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, size_type n, const T &val) {
  if (end_of_storage - finish >= n) {
    size_type m = finish - position;
    iterator old_finish = finish;
    if (m >= n) {
      uninitialized_copy(finish - n, finish, finish);
      finish += n;
      copy_backward(position, old_finish - n, old_finish);
      fill(position, position + n, val);
    } else {
      fill(position, old_finish, val);
      finish = uninitialized_fill_n(finish, n - m, val);
      finish = uninitialized_copy(position, old_finish, finish);
    }
    return position;
  } else {
    const size_type old_size = size();
    const size_type new_size = old_size + (n >= old_size ? n : old_size);
    const difference_type offset = position - begin();
    iterator new_start = data_allocator::allocate(new_size);
    iterator new_finish = uninitialized_copy(start, position, new_start);

    new_finish = uninitialized_fill_n(new_finish, n, val);
    new_finish = uninitialized_copy(position, finish, new_finish);

    destroy(begin(), end());
    deallocate();

    start = new_start;
    finish = new_finish;
    end_of_storage = start + new_size;
    return start + offset;
  }
}

} // namespace kirai
