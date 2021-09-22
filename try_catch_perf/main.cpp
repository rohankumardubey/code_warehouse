#include <iostream>
#include <string>
#include <sys/time.h>

const int NumRange = 1000;
int64_t GSuccCount = 0;
int64_t GFailCount = 0;

int get_rand(int range) {
  return rand() % range;
}

// 频繁 throw basic exception
void exit_with_basic_exception() {
  if (get_rand(2) == 0) {
    throw -1;
  }
}

// 频繁 throw message exception
void exit_with_message_exception_high() {
  if (get_rand(2) == 0) {
    throw std::bad_alloc();
  }
}

// 小概率 throw message exception
void exit_with_message_exception_low() {
  if (get_rand(1000) == 0) {
    throw std::bad_alloc();
  }
}

// 直接返回
void exit_with_return() {
  if (get_rand(2) == 0) {
    GFailCount++;
  } else {
    GSuccCount++;
  }
  return;
}

// 返回错误码
int exit_with_err_code() {
  if (get_rand(2) == 0) {
    return -1;
  }
  return 0;
}

void process_basic_exception() {
  try {
    exit_with_basic_exception();
    GSuccCount++;
  } catch (int ex) {
    GFailCount++;
  }
}

void process_message_exception_high() {
  try {
    exit_with_message_exception_high();
    GSuccCount++;
  } catch (std::exception& ex) {
    GFailCount++;
  }
}

void process_message_exception_low() {
  try {
    exit_with_message_exception_low();
    GSuccCount++;
  } catch (std::exception& ex) {
    GFailCount++;
  }
}

void process_void_return() {
  exit_with_return();
}

void process_err_return() {
  int err = exit_with_err_code();
  if (err == 0) {
    GFailCount++;
  } else {
    GSuccCount++;
  }
}

int64_t get_curtime() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

int main(int argc, char** argv) {
  int64_t start = 0;
  int64_t end = 0;
  int64_t count = 10000000;

  start = get_curtime();
  for (long i=0; i<count; i++) {
    process_basic_exception();
  }
  end = get_curtime();

  std::cout<<"BASIC_EXCEPTION: "<<(end-start)<<std::endl;
  std::cout<<"RESULT: "<<GSuccCount<<":"<<GFailCount<<std::endl;
  GSuccCount = 0;
  GFailCount = 0;

  start = get_curtime();
  for (long i=0; i<count; i++) {
    process_message_exception_high();
  }
  end = get_curtime();

  std::cout<<"HIGH_EXCEPTION: "<<(end-start)<<std::endl;
  std::cout<<"RESULT: "<<GSuccCount<<":"<<GFailCount<<std::endl;
  GSuccCount = 0;
  GFailCount = 0;

  start = get_curtime();
  for (long i=0; i<count; i++) {
    process_message_exception_low();
  }
  end = get_curtime();

  std::cout<<"LOW_EXCEPTION: "<<(end-start)<<std::endl;
  std::cout<<"RESULT: "<<GSuccCount<<":"<<GFailCount<<std::endl;
  GSuccCount = 0;
  GFailCount = 0;

  start = get_curtime();
  for (long i=0; i<count; i++) {
    process_void_return();
  }
  end = get_curtime();

  std::cout<<"VOID_RETURN: "<<(end-start)<<std::endl;
  std::cout<<"RESULT: "<<GSuccCount<<":"<<GFailCount<<std::endl;
  GSuccCount = 0;
  GFailCount = 0;

  start = get_curtime();
  for (long i=0; i<count; i++) {
    process_err_return();
  }
  end = get_curtime();

  std::cout<<"ERR_RETURN: "<<(end-start)<<std::endl;
  std::cout<<"RESULT: "<<GSuccCount<<":"<<GFailCount<<std::endl;
  GSuccCount = 0;
  GFailCount = 0;

  return 0;
}
