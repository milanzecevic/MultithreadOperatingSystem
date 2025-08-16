//
// Created by os on 7/19/25.
//

#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/tcb.hpp"
//#include "../h/print.hpp"

void userMain();

void shutdown() {
    volatile int* shutdownAdr = (int*) 0x100000;
    *shutdownAdr = 0x5555;
}

int main() {

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    thread_create(&TCB::running, nullptr, nullptr);

    userMain();

    return 0;
}

