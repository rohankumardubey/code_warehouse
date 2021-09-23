#include <iostream>
#include <cstdlib>
#include <malloc.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/nallocx.h>
#include <unistd.h>
#include <new>

#define ALIAS(tc_fn)   __attribute__ ((alias (#tc_fn), used))

struct MemTracker {
  int64_t sum;
};

struct MemTracker mem_tracker;

// auto* c1 = new C1();
void* operator new(size_t size) {
  void* p = tc_new(size);
  std::cout<<"NEW_WITH_SIZE_THROW: "<<p<<":"<<size<<":"<<tc_nallocx(size, 0)<<std::endl;
  return p;
}

// auto* c2 = new (std::nothrow) C1();
void* operator new(size_t size, const std::nothrow_t& nt) noexcept {
  void* p = tc_new_nothrow(size, nt);
  std::cout<<"NEW_WITH_SIZE_NO_THROW: "<<p<<":"<<size<<":"<<tc_nallocx(size, 0)<<std::endl;
  return p;
}

// delete c1
// > c++11
void operator delete(void* p, size_t size) noexcept {
  std::cout<<"FREE_WITH_SIZE_NO_THROW: "<<p<<":"<<size<<":"<<tc_nallocx(size, 0)<<std::endl;
  tc_delete_sized(p, size);
}

// c++11
void operator delete(void* p) noexcept {
  std::cout<<"FREE_NO_THROW: "<<p<<":"<<tc_malloc_size(p)<<std::endl;
  tc_delete(p);
}

void operator delete(void* p, const std::nothrow_t& nt) noexcept {
  std::cout<<"FREE_NO_THROW_2: "<<p<<":"<<tc_malloc_size(p)<<std::endl;
  tc_delete_nothrow(p, nt);
}

void* operator new[](size_t size) {
  void* p = tc_newarray(size);
  std::cout<<"NEW_ARRAY_WITH_SIZE_THROW: "<<p<<":"<<size<<":"<<tc_nallocx(size, 0)<<std::endl;
  return p;
}

void* operator new[](size_t size, const std::nothrow_t& nt) noexcept {
  void* p = tc_newarray_nothrow(size, nt);
  std::cout<<"NEW_ARRAY_WITH_SIZE_NOT_THROW: "<<p<<":"<<size<<":"<<tc_nallocx(size, 0)<<std::endl;
  return p;
}

void operator delete[](void* p) noexcept {
  std::cout<<"FREE_ARRAY_NO_THROW: "<<p<<":"<<tc_malloc_size(p)<<std::endl;
  tc_deletearray(p);
}

void operator delete[](void* p, size_t size) noexcept {
  std::cout<<"FREE_ARRAY_WITH_SIZE_NO_THROW: "<<p<<":"<<size<<":"<<tc_malloc_size(p)<<std::endl;
  tc_deletearray_sized(p, size);
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
  char* buf = (char*)malloc(1024);

  std::cout<<"================New With Exception================"<<std::endl;
  auto* c1 = new C1();
  delete c1;

  std::cout<<"================New Without Exception================"<<std::endl;
  auto* c2 = new (std::nothrow) C1();
  delete c2;

  std::cout<<"================New With Exception Align================"<<std::endl;
  auto* c3 = new (std::align_val_t{16}) C1();
  delete c3;

  std::cout<<"================New Array With Exception================"<<std::endl;
  auto* c4 = new C1[5];
  delete[] c4;

  std::cout<<"================New Array Without Exception================"<<std::endl;
  auto* c5 = new (std::nothrow) C1[5];
  delete[] c5;

  /*
  std::cout<<"START 3"<<std::endl;
  std::cout<<"T1:"<<mem_tracker.sum<<std::endl;
  void* c2 = malloc(10);
  std::cout<<"T2:"<<mem_tracker.sum<<std::endl;
  std::cout<<"T4:"<<tc_malloc_size(c2)<<std::endl;
  free(c2);
  std::cout<<"T3:"<<mem_tracker.sum<<std::endl;
  auto* c3 = new C1();
  */
}
