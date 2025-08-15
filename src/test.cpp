#include "../h/syscall_c.hpp"
#include "../h/print.hpp"

volatile bool thread1_done = false;
volatile bool thread2_done = false;
volatile int counter1 = 0;
volatile int counter2 = 0;

void workerThread1(void* arg) {
    for(int i = 0; i < 5; i++) {
        counter1 += 10;
        printString("Thread 1: iteration ");
        printInteger(i);
        printString(", counter1 = ");
        printInteger(counter1);
        printString("\n");

        // Yield to other threads
        thread_dispatch();
    }
    thread1_done = true;
    printString("Thread 1 finished!\n");
    thread_exit();
}

void workerThread2(void* arg) {
    for(int i = 0; i < 5; i++) {
        counter2 += 20;
        printString("Thread 2: iteration ");
        printInteger(i);
        printString(", counter2 = ");
        printInteger(counter2);
        printString("\n");

        // Yield to other threads
        thread_dispatch();
    }
    thread2_done = true;
    printString("Thread 2 finished!\n");
    thread_exit();
}

void threadTest() {
    printString("Starting thread test...\n");

    // Create worker threads using syscall_c API
    thread_t thread1;
    thread_t thread2;

    int ret1 = thread_create(&thread1, workerThread1, nullptr);
    if(ret1 == 0) {
        printString("Thread 1 created successfully\n");
    } else {
        printString("Failed to create thread 1\n");
    }

    int ret2 = thread_create(&thread2, workerThread2, nullptr);
    if(ret2 == 0) {
        printString("Thread 2 created successfully\n");
    } else {
        printString("Failed to create thread 2\n");
    }

    // Start thread execution by dispatching
    printString("Starting thread execution...\n");

    // Main thread yields to let other threads run
    while(!thread1_done || !thread2_done) {
        thread_dispatch();
    }

    printString("\nBoth threads completed!\n");
    printString("Final counter1: ");
    printInteger(counter1);
    printString("\nFinal counter2: ");
    printInteger(counter2);
    printString("\n");
}