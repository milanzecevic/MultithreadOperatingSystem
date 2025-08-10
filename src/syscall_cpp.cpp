//
// Created by os on 8/10/25.
//

#include "../h/syscall_cpp.hpp"

void* operator new (size_t n) {
    return mem_alloc(n);
}


void* operator new[](size_t n) {
    return mem_alloc(n);
}


void  operator delete (void* p) noexcept {
    mem_free(p);
}


void operator delete[](void *p) noexcept {
    mem_free(p);
}


