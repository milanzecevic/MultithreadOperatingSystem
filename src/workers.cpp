//
// Created by os on 8/14/25.
//

#include "../h/workers.hpp"

#include "../h/print.hpp"

static volatile bool finishedA = false;
static volatile bool finishedB = false;
static volatile bool finishedC = false;
static volatile bool finishedD = false;

uint64 fibonacci(uint64 n) {
    if (n == 0 || n == 1) { return n; }
    if (n % 10 == 0) { thread_dispatch(); }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void workerBodyA(void* arg) {
    for (uint64 i = 0; i < 10; i++) {
        printString("A: i="); printInteger(i); printString("\n");
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 30000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
    printString("A finished!\n");
    finishedA = true;
}

void workerBodyB(void* arg) {
    for (uint64 i = 0; i < 16; i++) {
        printString("B: i="); printInteger(i); printString("\n");
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 30000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
    printString("B finished!\n");
    finishedB = true;
    thread_dispatch();
}

void workerBodyC(void* arg) {
    uint8 i = 0;
    for (; i < 3; i++) {
        printString("C: i="); printInteger(i); printString("\n");
    }

    printString("C: dispatch\n");
    __asm__ ("li t1, 7");
    thread_dispatch();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    printString("C: t1="); printInteger(t1); printString("\n");

    uint64 result = fibonacci(12);
    printString("C: fibonaci="); printInteger(result); printString("\n");

    for (; i < 6; i++) {
        printString("C: i="); printInteger(i); printString("\n");
    }

    printString("C finished!\n");
    finishedC = true;
    thread_dispatch();
}

void workerBodyD(void* arg) {
    uint8 i = 10;
    for (; i < 13; i++) {
        printString("D: i="); printInteger(i); printString("\n");
    }

    printString("D: dispatch\n");
    __asm__ ("li t1, 5");
    thread_dispatch();

    uint64 result = fibonacci(16);
    printString("D: fibonaci="); printInteger(result); printString("\n");

    for (; i < 16; i++) {
        printString("D: i="); printInteger(i); printString("\n");
    }

    printString("D finished!\n");
    finishedD = true;
    thread_dispatch();
}