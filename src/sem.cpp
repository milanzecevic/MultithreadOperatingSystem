//
// Created by os on 8/15/25.
//

#include "../h/sem.hpp"

Semaphore *Semaphore::createSem(uint64 val) {
    return new Semaphore(val);
}

void Semaphore::sem_close() {
    TCB* curr = 0;
    while (blockedThreadQueue.peekFirst()){
        curr = blockedThreadQueue.removeFirst();
        Scheduler::put(curr);
    }
}

void Semaphore::sem_wait() {
    this->val--;
    if(this->val < 0) {
        block();
    }
}

void Semaphore::sem_signal() {
    this->val++;
    if(this->val <= 0) {
        unblock();
    }
}

int Semaphore::sem_trywait() {
    val--;
    if(val >= 0) {
        return 0;
    } else {
        val++;
        return 1;
    }
}

void Semaphore::block() {
    blockedThreadQueue.addLast(TCB::running);
    // TCB::running->state = TCB::BLOCKED;
    TCB::running->setBlocked(true);
    TCB::dispatch();
}

void Semaphore::unblock() {
    TCB* thread = blockedThreadQueue.removeFirst();
    if(thread != nullptr) {
        //     TCB::running->state = TCB::READY;
        thread->setBlocked(false);
        Scheduler::put(thread);
    }
}
