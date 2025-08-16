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

}

int Thread::start() {
    if (this->body) {
        thread_create(&myHandle, this->body, this->arg);
    } else {
        thread_create(&myHandle, reinterpret_cast<void (*)(void *)>(runWrapper), this);
    }
    return 1;
}

void Thread::dispatch() {
    thread_dispatch();
}

Thread::Thread() : myHandle(nullptr), body(nullptr), arg(nullptr){

}

void Thread::runWrapper(Thread *thread) {
    thread->run();
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


char Console::getc() {
    return ::getc();
}

void Console::putc(char character) {
    ::putc(character);
}
