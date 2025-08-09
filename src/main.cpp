//
// Created by os on 7/19/25.
//

#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/print.hpp"

int main() {

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    // 1. Alokacija memorije
    void* ptr = mem_alloc(64);
    if (!ptr) {
        printString("mem_alloc failed\n");
        return -1;
    }
    printString("mem_alloc success, address: ");
    printInteger((uint64)ptr);
    printString("\n");

    // 2. Pisanje test vrednosti (recimo broj 123 u prvi bajt)
    //((char*)ptr)[0] = 123;

    // 3. Ispis vrednosti iz memorije
    printString("First byte value: ");

    //printInteger((uint64)((unsigned char*)ptr)[0]);
    printString("\n");

    // 4. Oslobađanje memorije
    int free_result = mem_free(ptr);
    printString("mem_free result: ");
    printInteger((uint64)free_result);
    printString("\n");

    return 0;
}