#ifndef KIRAI_LIB_ALLOCATOR_H
#define KIRAI_LIB_ALLOCATOR_H

#include <cstdlib>

namespace kirai {

class __alloc_large {

 public:
  static void* allocate(size_t n);

  static void deallocate(void* p, size_t n);

  static void* reallocate(void* p, size_t n);

  static void (*set_malloc_handler(void(*f)()))() {
    void (*old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;
    return old;
  }

 private:
  static void* oom_malloc(size_t n);

  static void* oom_realloc(void* p, size_t n);

  static void (*__malloc_alloc_oom_handler)();
};


class alloc {
 public:
  static void* allocate(size_t bytes);

  static void deallocate(void* p, size_t bytes);

  static void* reallocate(size_t bytes);

 private:
  typedef __alloc_large malloc_alloc;
  enum { __ALIGN = 8 };
  enum { __MAX_BYTES = 128 };
  enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

  union obj {
    union obj* free_list_link;
    char data[1];
  };

 private:
  static obj* free_list[__NFREELISTS];
  static char* start_free;
  static char* end_free;
  static size_t heap_size;

 private:
  static size_t ROUND_UP(size_t bytes);

  static size_t FREELIST_INDEX(size_t bytes);

  static void* refill(size_t bytes);

  static void* chunk_alloc(size_t bytes, size_t& nobjs);
};

template<typename T, typename Alloc>
class simple_alloc {
public:
  static T* allocate(size_t n) {
    return 0 == n ? 0 : static_cast<T*>(Alloc::allocate(n * sizeof(T)));
  }
  static T* allocate() {
    return static_cast<T*>(Alloc::allocate(sizeof(T)));
  }
  static void deallocate(T* p, size_t n) {
    if (0 != n) {
      Alloc::deallocate(p, sizeof(T) * n);
    }
  }
  static void deallocate(T* p) {
    Alloc::deallocate(p, sizeof(T));
  }
};


} // namespace kirai

#endif