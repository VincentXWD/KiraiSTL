#include "allocator.h"

#ifndef __THROW_BAD_ALLOC
#include <iostream> 
#include <unistd.h>
#define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1);
#endif

namespace kirai {

/**
 * __alloc_large begin
*/ 
void* __alloc_large::allocate(size_t n) {
  void* ret = malloc(n);
  if (ret == 0) ret = oom_malloc(n);
  return ret;
}

void __alloc_large::deallocate(void* p, size_t n) {
  free(p);
}

void* __alloc_large::reallocate(void* p, size_t n) {
  void* ret = realloc(p, n);
  if (ret == 0) ret = oom_realloc(p, n);
  return ret;
}

void (*__alloc_large::__malloc_alloc_oom_handler)() = 0;

void* __alloc_large::oom_malloc(size_t n) {
  void (*handler)();
  void* ret;
  while (true) {
    handler = __malloc_alloc_oom_handler;
    if (handler == 0) __THROW_BAD_ALLOC;
    (*handler)();
    ret = malloc(n);
    if(ret) return ret;
  }
}

void* __alloc_large::oom_realloc(void* p, size_t n) {
  void (*handler)();
  void* ret;
  while (true) {
    handler = __malloc_alloc_oom_handler;
    if (handler == 0) __THROW_BAD_ALLOC;
    (*handler)();
    ret = realloc(p, n);
    if(ret) return ret;
  }
}

/**
 * alloc begin
*/
void* alloc::allocate(size_t bytes) {
  obj** my_free_list;
  obj* result;

  if (bytes > (size_t)__MAX_BYTES) {
    return malloc_alloc::allocate(bytes);
  }

  my_free_list = free_list + FREELIST_INDEX(bytes);
  result = *my_free_list;
  if (result == 0) {
    void* r = refill(ROUND_UP(bytes));
    return r;
  }
  *my_free_list = result->free_list_link;
  return result;
}

void alloc::deallocate(void* p, size_t bytes) {
  obj** my_free_list;
  if (bytes > (size_t)__MAX_BYTES) {
    return malloc_alloc::deallocate(p, bytes);
  }

  my_free_list = free_list + FREELIST_INDEX(bytes);
  obj * q = (obj*)p;
  q->free_list_link = *my_free_list;
  *my_free_list = q;
}

size_t alloc::ROUND_UP(size_t bytes) {
  return (bytes + __ALIGN - 1) & ~(__ALIGN - 1);
}

size_t alloc::FREELIST_INDEX(size_t bytes) {
  return ((bytes + __ALIGN - 1) / __ALIGN - 1);
}

void* alloc::refill(size_t bytes) {
  size_t nobjs = 20;
  char* chunk = (char*)chunk_alloc(bytes, nobjs);

  obj** my_free_list;
  if (nobjs == 1) return chunk;
  void* ret = chunk;
  my_free_list = free_list + FREELIST_INDEX(bytes);
  obj* next = (obj*)(chunk + bytes);
  *my_free_list = next;
  obj* cur;
  for (int i = 1; ; i++) {
    cur = next;
    next = (obj*)(char*)(next + bytes);
    if (i != nobjs - 1) {
      cur->free_list_link = next;
    } else {
      cur->free_list_link = 0;
      break;
    }
  }

  return ret;
}

void* alloc::chunk_alloc(size_t bytes, size_t& nobjs) {
  char* result;

  size_t total_size = bytes * nobjs;
  size_t bytes_left = end_free - start_free;

  if (bytes_left >= total_size) {
    result = start_free;
    start_free += total_size;
    return result;
  } else if (bytes_left >= bytes) {
    nobjs = bytes_left / bytes;
    total_size = bytes * nobjs;
    result = start_free;
    start_free += total_size;
    return result;
  } else {
    size_t bytes_to_get = 2 * total_size + ROUND_UP(heap_size >> 4);
    if (bytes_left > 0) {
      obj** my_free_list = free_list + FREELIST_INDEX(bytes_left);
      obj* q = (obj*)start_free;
      q->free_list_link = *my_free_list;
      *my_free_list = q;
    }

    start_free = (char*)malloc(bytes_to_get);
    if (start_free == 0) {
      obj** my_free_list;
      obj* p;
      for (int i = bytes; i <= __MAX_BYTES; i = i + __ALIGN) {
        my_free_list = free_list + ROUND_UP(i);
        p = *my_free_list;
        if (p) {
          start_free = (char*)p;
          end_free = start_free + i;
          *my_free_list = p->free_list_link;
          return chunk_alloc(bytes, nobjs);
        }
        end_free = 0;
        start_free = (char*)malloc_alloc::allocate(bytes_to_get);
      }
      heap_size += bytes_to_get;
      end_free = start_free + bytes_to_get;
      return chunk_alloc(bytes, nobjs);
    }
  }
}

} // namespace kirai
