#include <iostream>
#include <cstdlib>
#include <malloc.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/nallocx.h>
#include <unistd.h>
#include <sys/time.h>
#include <vector>

#define ALIAS(tc_fn)   __attribute__ ((alias (#tc_fn), used))

const int NumRange = 4;

int get_rand(int range) {
  return rand() % range;
}

class C1 {
private:
  int64_t _a1;
};

class C2 {
private:
  int64_t _a1;
  int64_t _a2;
};

class C3 {
private:
  int64_t _a1;
  int64_t _a2;
  int64_t _a3;
  int64_t _a4;
};

class C4 {
private:
  int64_t _a1;
  int64_t _a2;
  int64_t _a3;
  int64_t _a4;
  int64_t _a5;
  int64_t _a6;
  int64_t _a7;
  int64_t _a8;
};


struct MemTracker {
  int64_t sum;
};

struct MemTracker mem_tracker;
std::vector<void*> values;

//void* operator new(size_t size) {
//  mem_tracker.sum += size;
//  void* p = tc_malloc(size);
//  return p;
//}
//
//void operator delete(void* p) noexcept {
//  tc_free(p);
//}
//
//void operator delete(void* p, size_t size) noexcept {
//  mem_tracker.sum -= size;
//  tc_delete_sized(p, size);
//}
//
//extern "C" {
//  void* my_alloc(size_t size) {
//    size_t new_size = nallocx(size, 0);
//    mem_tracker.sum += new_size;
//    void* p = tc_malloc(size);
//    return p;
//  }
//
//  void my_free(void* p) {
//    size_t size = tc_malloc_size(p);
//    mem_tracker.sum -= size;
//    tc_free(p);
//  }
//}
//
//extern "C" {
//  void* malloc(size_t size) __THROW ALIAS(my_alloc);
//  void free(void* p) __THROW ALIAS(my_free);
//}

int64_t get_curtime() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

void new_class() {
  for (size_t i=0; i<30000000; i++) {
    int v = get_rand(4);
    void* p = nullptr;
    if (v == 0) {
      p = (void*) new C1();
      mem_tracker.sum += sizeof(C1);
    } else if (v == 1) {
      p = (void*) new C2();
      mem_tracker.sum += sizeof(C2);
    } else if (v == 2) {
      p = (void*) new C3();
      mem_tracker.sum += sizeof(C3);
    } else {
      p = (void*) new C4();
      mem_tracker.sum += sizeof(C4);
    }
    values.push_back(p);
  }
}

void malloc_class() {
  for (size_t i=0; i<10000000; i++) {
    int v = get_rand(4);
    void* p = nullptr;
    if (v == 0) {
      p = malloc(32);
    } else if (v == 1) {
      p = malloc(64);
    } else if (v == 2) {
      p = malloc(128);
    } else {
      p = malloc(256);
    }
    values.push_back(p);
  }
}

int main() {
  int64_t start = get_curtime();
  new_class();
  int64_t end = get_curtime();
  std::cout<<"RESULT:"<<(end-start)<<std::endl;
  std::cout<<"MEM:"<<mem_tracker.sum<<std::endl;

  start = get_curtime();
  malloc_class();
  end = get_curtime();
  std::cout<<"RESULT:"<<(end-start)<<std::endl;
  std::cout<<"MEM:"<<mem_tracker.sum<<std::endl;
}
