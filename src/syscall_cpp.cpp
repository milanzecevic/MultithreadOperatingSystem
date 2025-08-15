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
    this->body = body;
    this->arg = arg;
    myHandle = nullptr;
}

Thread::~Thread() {
    thread_exit();
}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

Thread::Thread() {
    this->body = nullptr;
    this->arg = nullptr;
    myHandle = nullptr;
}


Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}


