//
// Created by os on 7/19/25.
//

#include "../h/riscv.hpp"

int main() {

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);


}