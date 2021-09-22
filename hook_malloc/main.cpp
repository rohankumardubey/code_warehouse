#include <iostream>
#include <cstdlib>
#include <malloc.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/nallocx.h>
#include <unistd.h>

#define ALIAS(tc_fn)   __attribute__ ((alias (#tc_fn), used))

struct MemTracker {
  int64_t sum;
};

struct MemTracker mem_tracker;

void* operator new(size_t size) {
  void* p = tc_malloc(size);
  std::cout<<"NEW: "<<p<<":"<<size<<":"<<tc_nallocx(size, 0)<<std::endl;
  return p;
}

void operator delete(void* p) noexcept {
  std::cout<<"FREE: "<<p<<":"<<tc_malloc_size(p)<<std::endl;
  tc_free(p);
}

void operator delete(void* p, size_t size) noexcept {
  std::cout<<"FREE_SIZE: "<<p<<":"<<size<<std::endl;
  tc_delete_sized(p, size);
}

extern "C" {
  void* my_alloc(size_t size) {
    size_t new_size = nallocx(size, 0);
    void* p = tc_malloc(size);
    mem_tracker.sum += new_size;
    return p;
  }

  void my_free(void* p) {
    size_t size = tc_malloc_size(p);
    tc_free(p);
    mem_tracker.sum -= size;
  }
}

extern "C" {
  void* malloc(size_t size) __THROW ALIAS(my_alloc);
  void free(void* p) __THROW ALIAS(my_free);
}

class C1 {
public:
  void output() { }

private:
  int _a1;
  int _a2;
  int8_t _a3;
};

int main() {
  std::cout<<"====================================="<<std::endl;
  std::cout<<"START 1"<<std::endl;
  auto* c1 = new C1();
  std::cout<<"START 2"<<std::endl;
  delete c1;
  std::cout<<"START 3"<<std::endl;
  std::cout<<"T1:"<<mem_tracker.sum<<std::endl;
  void* c2 = malloc(10);
  std::cout<<"T2:"<<mem_tracker.sum<<std::endl;
  std::cout<<"T4:"<<tc_malloc_size(c2)<<std::endl;
  free(c2);
  std::cout<<"T3:"<<mem_tracker.sum<<std::endl;
  auto* c3 = new C1();
}
