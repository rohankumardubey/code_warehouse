g++ main.cpp -ltcmalloc -lpthread

```
vim src/libc_override_gcc_and_weak.h

// 注释掉下面的SQL

// void* operator new(size_t size) CPP_BADALLOC  ALIAS(tc_new);
// void operator delete(void* p) CPP_NOTHROW     ALIAS(tc_delete);
// void* malloc(size_t size) __THROW               ALIAS(tc_malloc); 
// void free(void* ptr) __THROW                    ALIAS(tc_free);
```

