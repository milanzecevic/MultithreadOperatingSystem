//
// Advanced Semaphore Tests with Multiple Threads
// Producer-Consumer and Thread Synchronization Tests
//

#include "../h/syscall_c.h"
#include "../h/print.hpp"

// Simple output functions
void print(const char* str) {
    printStringMoj(str);
}

void printNum(int num) {
    printIntMoj((uint64)num);
}

// ============================================
// Test 1: Multiple Threads with Mutual Exclusion
// ============================================

sem_t mutex_sem;
int shared_counter = 0;

void counter_thread(void* arg) {
    int thread_id = (int)(uint64)arg;

    for (int i = 0; i < 5; i++) {
        // Enter critical section
        sem_wait(mutex_sem);

        // Critical section: increment shared counter
        int local = shared_counter;
        print("Thread ");
        printNum(thread_id);
        print(" read counter: ");
        printNum(local);
        print("\n");

        // Simulate some work
        for (volatile int j = 0; j < 50000; j++);

        shared_counter = local + 1;
        print("Thread ");
        printNum(thread_id);
        print(" updated counter to: ");
        printNum(shared_counter);
        print("\n");

        // Exit critical section
        sem_signal(mutex_sem);

        // Yield to other threads
        thread_dispatch();
    }

    thread_exit();
}

void test_mutex() {
    print("\n=== Test 1: Mutex with Multiple Threads ===\n");

    shared_counter = 0;
    sem_open(&mutex_sem, 1);  // Binary semaphore for mutual exclusion

    thread_t threads[4];

    print("Creating 4 threads to increment shared counter...\n");
    for (int i = 0; i < 4; i++) {
        thread_create(&threads[i], counter_thread, (void*)(uint64)(i + 1));
    }

    // Let threads run
    print("Main thread: waiting for threads to complete...\n");
    for (volatile int i = 0; i < 5000000; i++) {
        if (i % 1000000 == 0) thread_dispatch();
    }

    print("Final counter value: ");
    printNum(shared_counter);
    print(" (expected: 20)\n");

    sem_close(mutex_sem);
}

// ============================================
// Test 2: Producer-Consumer Pattern
// ============================================

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

sem_t empty_slots;
sem_t full_slots;
sem_t buffer_mutex;
int buffer[BUFFER_SIZE];
int in_index = 0;
int out_index = 0;
int total_produced = 0;
int total_consumed = 0;

void producer(void* arg) {
    int producer_id = (int)(uint64)arg;

    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = producer_id * 100 + i;

        // Wait for empty slot
        sem_wait(empty_slots);

        // Lock buffer
        sem_wait(buffer_mutex);

        // Produce item
        buffer[in_index] = item;
        print("Producer ");
        printNum(producer_id);
        print(" produced item ");
        printNum(item);
        print(" at index ");
        printNum(in_index);
        print("\n");

        in_index = (in_index + 1) % BUFFER_SIZE;
        total_produced++;

        // Unlock buffer
        sem_signal(buffer_mutex);

        // Signal item available
        sem_signal(full_slots);

        // Yield to consumers
        thread_dispatch();
    }

    print("Producer ");
    printNum(producer_id);
    print(" finished\n");
    thread_exit();
}

void consumer(void* arg) {
    int consumer_id = (int)(uint64)arg;

    for (int i = 0; i < NUM_ITEMS; i++) {
        // Wait for full slot
        sem_wait(full_slots);

        // Lock buffer
        sem_wait(buffer_mutex);

        // Consume item
        int item = buffer[out_index];
        print("Consumer ");
        printNum(consumer_id);
        print(" consumed item ");
        printNum(item);
        print(" from index ");
        printNum(out_index);
        print("\n");

        out_index = (out_index + 1) % BUFFER_SIZE;
        total_consumed++;

        // Unlock buffer
        sem_signal(buffer_mutex);

        // Signal empty slot
        sem_signal(empty_slots);

        // Yield to producers
        thread_dispatch();
    }

    print("Consumer ");
    printNum(consumer_id);
    print(" finished\n");
    thread_exit();
}

void test_producer_consumer() {
    print("\n=== Test 2: Producer-Consumer Pattern ===\n");

    // Reset globals
    in_index = 0;
    out_index = 0;
    total_produced = 0;
    total_consumed = 0;

    // Initialize semaphores
    sem_open(&empty_slots, BUFFER_SIZE);  // All slots empty initially
    sem_open(&full_slots, 0);             // No items initially
    sem_open(&buffer_mutex, 1);           // Binary semaphore for buffer access

    print("Buffer size: ");
    printNum(BUFFER_SIZE);
    print("\n");

    thread_t producer1, producer2;
    thread_t consumer1, consumer2;

    print("Creating 2 producers and 2 consumers...\n");
    thread_create(&producer1, producer, (void*)1);
    thread_create(&producer2, producer, (void*)2);
    thread_create(&consumer1, consumer, (void*)1);
    thread_create(&consumer2, consumer, (void*)2);

    // Let threads run
    print("Main thread: waiting for completion...\n");
    for (volatile int i = 0; i < 10000000; i++) {
        if (i % 1000000 == 0) thread_dispatch();
    }

    print("\nResults:\n");
    print("Total produced: ");
    printNum(total_produced);
    print("\n");
    print("Total consumed: ");
    printNum(total_consumed);
    print("\n");

    sem_close(empty_slots);
    sem_close(full_slots);
    sem_close(buffer_mutex);
}

// ============================================
// Test 3: Readers-Writers Problem
// ============================================

sem_t rw_mutex;      // Mutual exclusion for writers
sem_t read_mutex;    // Protect read_count
int read_count = 0;
int shared_data = 0;

void reader_thread(void* arg) {
    int reader_id = (int)(uint64)arg;

    for (int i = 0; i < 3; i++) {
        // Entry section for readers
        sem_wait(read_mutex);
        read_count++;
        if (read_count == 1) {
            sem_wait(rw_mutex);  // First reader locks out writers
        }
        sem_signal(read_mutex);

        // Critical section: read shared data
        print("Reader ");
        printNum(reader_id);
        print(" read value: ");
        printNum(shared_data);
        print("\n");

        // Simulate reading time
        for (volatile int j = 0; j < 100000; j++);

        // Exit section for readers
        sem_wait(read_mutex);
        read_count--;
        if (read_count == 0) {
            sem_signal(rw_mutex);  // Last reader unlocks writers
        }
        sem_signal(read_mutex);

        thread_dispatch();
    }

    thread_exit();
}

void writer_thread(void* arg) {
    int writer_id = (int)(uint64)arg;

    for (int i = 0; i < 2; i++) {
        // Entry section for writers
        sem_wait(rw_mutex);

        // Critical section: write shared data
        shared_data++;
        print("Writer ");
        printNum(writer_id);
        print(" wrote value: ");
        printNum(shared_data);
        print("\n");

        // Simulate writing time
        for (volatile int j = 0; j < 150000; j++);

        // Exit section for writers
        sem_signal(rw_mutex);

        thread_dispatch();
    }

    thread_exit();
}

void test_readers_writers() {
    print("\n=== Test 3: Readers-Writers Problem ===\n");

    // Reset globals
    read_count = 0;
    shared_data = 0;

    // Initialize semaphores
    sem_open(&rw_mutex, 1);
    sem_open(&read_mutex, 1);

    thread_t readers[3];
    thread_t writers[2];

    print("Creating 3 readers and 2 writers...\n");

    // Create readers
    for (int i = 0; i < 3; i++) {
        thread_create(&readers[i], reader_thread, (void*)(uint64)(i + 1));
    }

    // Create writers
    for (int i = 0; i < 2; i++) {
        thread_create(&writers[i], writer_thread, (void*)(uint64)(i + 1));
    }

    // Let threads run
    print("Main thread: waiting for completion...\n");
    for (volatile int i = 0; i < 8000000; i++) {
        if (i % 1000000 == 0) thread_dispatch();
    }

    print("Final shared data value: ");
    printNum(shared_data);
    print("\n");

    sem_close(rw_mutex);
    sem_close(read_mutex);
}

// ============================================
// Test 4: Barrier Synchronization
// ============================================

#define NUM_THREADS 4

sem_t barrier_mutex;
sem_t barrier_sem;
int barrier_count = 0;

void barrier_thread(void* arg) {
    int thread_id = (int)(uint64)arg;

    // Phase 1: Do some work
    print("Thread ");
    printNum(thread_id);
    print(" starting phase 1\n");

    for (volatile int i = 0; i < 200000; i++);

    print("Thread ");
    printNum(thread_id);
    print(" finished phase 1, waiting at barrier\n");

    // Barrier synchronization
    sem_wait(barrier_mutex);
    barrier_count++;
    if (barrier_count == NUM_THREADS) {
        // Last thread to arrive releases all others
        print("Thread ");
        printNum(thread_id);
        print(" is last, releasing all threads\n");
        for (int i = 0; i < NUM_THREADS - 1; i++) {
            sem_signal(barrier_sem);
        }
        barrier_count = 0;  // Reset for potential reuse
        sem_signal(barrier_mutex);
    } else {
        sem_signal(barrier_mutex);
        sem_wait(barrier_sem);  // Wait for all threads
    }

    // Phase 2: Continue after barrier
    print("Thread ");
    printNum(thread_id);
    print(" starting phase 2\n");

    for (volatile int i = 0; i < 100000; i++);

    print("Thread ");
    printNum(thread_id);
    print(" finished phase 2\n");

    thread_exit();
}

void test_barrier() {
    print("\n=== Test 4: Barrier Synchronization ===\n");

    barrier_count = 0;

    // Initialize semaphores
    sem_open(&barrier_mutex, 1);
    sem_open(&barrier_sem, 0);

    thread_t threads[NUM_THREADS];

    print("Creating ");
    printNum(NUM_THREADS);
    print(" threads with barrier sync...\n");

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_create(&threads[i], barrier_thread, (void*)(uint64)(i + 1));
    }

    // Let threads run
    print("Main thread: waiting for completion...\n");
    for (volatile int i = 0; i < 5000000; i++) {
        if (i % 500000 == 0) thread_dispatch();
    }

    print("All threads completed barrier synchronization\n");

    sem_close(barrier_mutex);
    sem_close(barrier_sem);
}

// ============================================
// Test 5: Dining Philosophers (Simplified)
// ============================================

#define NUM_PHILOSOPHERS 3

sem_t forks[NUM_PHILOSOPHERS];
sem_t dining_mutex;  // To prevent deadlock

void philosopher(void* arg) {
    int id = (int)(uint64)arg;
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;

    for (int i = 0; i < 2; i++) {
        // Think
        print("Philosopher ");
        printNum(id);
        print(" is thinking\n");
        for (volatile int j = 0; j < 100000; j++);
        thread_dispatch();

        // Pick up forks (with mutex to prevent deadlock)
        sem_wait(dining_mutex);

        sem_wait(forks[left_fork]);
        print("Philosopher ");
        printNum(id);
        print(" picked up left fork\n");

        sem_wait(forks[right_fork]);
        print("Philosopher ");
        printNum(id);
        print(" picked up right fork\n");

        sem_signal(dining_mutex);

        // Eat
        print("Philosopher ");
        printNum(id);
        print(" is eating\n");
        for (volatile int j = 0; j < 150000; j++);

        // Put down forks
        sem_signal(forks[left_fork]);
        sem_signal(forks[right_fork]);
        print("Philosopher ");
        printNum(id);
        print(" put down forks\n");

        thread_dispatch();
    }

    print("Philosopher ");
    printNum(id);
    print(" finished dining\n");
    thread_exit();
}

void test_dining_philosophers() {
    print("\n=== Test 5: Dining Philosophers ===\n");

    // Initialize fork semaphores
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_open(&forks[i], 1);
    }
    sem_open(&dining_mutex, 1);

    thread_t philosophers[NUM_PHILOSOPHERS];

    print("Creating ");
    printNum(NUM_PHILOSOPHERS);
    print(" philosophers...\n");

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        thread_create(&philosophers[i], philosopher, (void*)(uint64)i);
    }

    // Let threads run
    print("Main thread: waiting for philosophers...\n");
    for (volatile int i = 0; i < 8000000; i++) {
        if (i % 1000000 == 0) thread_dispatch();
    }

    print("All philosophers finished\n");

    // Clean up
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_close(forks[i]);
    }
    sem_close(dining_mutex);
}

// Main test function
void sem_test2_main() {
    print("\n========================================\n");
    print("   ADVANCED SEMAPHORE TESTS WITH THREADS\n");
    print("========================================\n");

    test_mutex();
    test_producer_consumer();
    test_readers_writers();
    test_barrier();
    test_dining_philosophers();

    print("\n========================================\n");
    print("     ALL ADVANCED TESTS COMPLETED\n");
    print("========================================\n");
}