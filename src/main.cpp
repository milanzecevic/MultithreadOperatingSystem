//
// Created by os on 7/19/25.
//

#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/tcb.hpp"
//#include "../h/print.hpp"
#include "../h/Threads_C_API_test.hpp"
#include "../h/workers.hpp"

void shutdown() {
    volatile int* shutdownAdr = (int*) 0x100000;
    *shutdownAdr = 0x5555;
}

void simpleThread(void* arg) {
    // Thread body - won't execute without dispatch
}

int main() {

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    TCB *threads[5];

    thread_create(&threads[0],nullptr, nullptr);
    TCB::running = threads[0];

    thread_create(&threads[1], workerBodyA, nullptr);
    printString("ThreadA created\n");
    thread_create(&threads[2], workerBodyB, nullptr);
    printString("ThreadB created\n");
    thread_create(&threads[3], workerBodyC, nullptr);
    printString("ThreadC created\n");
    thread_create(&threads[4], workerBodyD, nullptr);
    printString("ThreadD created\n");

    while (!(threads[1]->isFinished() &&
            threads[2]->isFinished() &&
            threads[3]->isFinished() &&
            threads[4]->isFinished())) {
        thread_dispatch();
    }

    printString("Finished\n");

    return 0;
}

