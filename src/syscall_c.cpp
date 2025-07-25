//
// Created by os on 7/19/25.
//

#include "../h/syscall_c.hpp"

void *mem_alloc(size_t size) {
    size_t newSize = size/MEM_BLOCK_SIZE;
    if(size%MEM_BLOCK_SIZE != 0) {
        newSize += 1;
    }
    void* returnAdr = nullptr;
    __asm__ volatile("mv a1, %0" : : "r"(newSize));
    __asm__ volatile("mv a0, %0" : : "r"(0x01));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(returnAdr));
    return returnAdr;
}

int nen_free(void *adrPtr) {
    int retVal = -1;
    __asm__ volatile("mv a1, %0" : : "r"(adrPtr));
    __asm__ volatile("mv a0, %0" : : "r"(0x02));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(retVal));
    return retVal;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    int retVal = -1;
    __asm__ volatile("mv a3, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"(start_routine));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("mv a0, %0" : : "r"(0x11));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(retVal));
    return retVal;
}

int thread_exit() {
    int retVal = -1;
    __asm__ volatile("mv a0, %0" : : "r"(0x12));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(retVal));
    return retVal;
}

void thread_dispatch() {
    __asm__ volatile("mv a0, %0" : : "r"(0x13));
    __asm__ volatile("ecall");
}

int sem_open(sem_t *handle, unsigned int init) {
    int retVal = -1;
    __asm__ volatile("mv a2, %0" : : "r"(init));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("mv a0, %0" : : "r"(0x21));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(retVal));
    return retVal;
}

int sem_close(sem_t handle) {
    int retVal = -1;
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("mv a0, %0" : : "r"(0x22));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(retVal));
    return retVal;
}

int sem_wait(sem_t id) {
    int retVal = -1;
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(0x23));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(retVal));
    return retVal;
}

int sem_signal(sem_t id) {
    int retVal = -1;
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(0x24));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(retVal));
    return retVal;
}

int sem_trywait(sem_t id) {
    int retVal = -1;
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(0x26));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(retVal));
    return retVal;
}

char getc() {
    char c;
    __asm__ volatile("mv a0, %0" : : "r"(0x41));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r"(c));
    return c;
}

void putc(char c) {
    __asm__ volatile("mv a1, %0" : : "r"(c));
    __asm__ volatile("mv a0, %0" : : "r"(0x42));
    __asm__ volatile("ecall");
}
