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


Thread::Thread(void (*body)(void *), void *arg) {

}

Thread::~Thread() {

}

int Thread::start() {
    return 0;
}

void Thread::dispatch() {

}

Thread::Thread() {

}


Semaphore::Semaphore(unsigned int init) {

}

Semaphore::~Semaphore() {

}

int Semaphore::wait() {
    return 0;
}

int Semaphore::signal() {
    return 0;
}

int Semaphore::tryWait() {
    return 0;
}


