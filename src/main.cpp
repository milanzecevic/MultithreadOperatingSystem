//
// Created by os on 7/19/25.
//

#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/print.hpp"

void shutdown() {
    volatile int* shutdownAdr = (int*) 0x100000;
    *shutdownAdr = 0x5555;
}

int main() {

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    shutdown();

    return 0;
}