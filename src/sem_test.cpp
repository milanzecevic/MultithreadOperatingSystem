// Additional test cases for semaphores and producer-consumer patterns
// These tests complement TEST_3 from the official test suite

#include "../h/syscall_c.h"
#include "../test/printing.hpp"

// ============================================================
// TEST 1: Simple Binary Semaphore (Mutex) Test
// ============================================================
void test_binary_semaphore() {
    printString("\n=== Binary Semaphore Test ===\n");

    sem_t mutex;
    volatile int shared_counter = 0;
    volatile int thread_finished[3] = {0, 0, 0};

    struct thread_data {
        sem_t* mutex;
        volatile int* counter;
        int thread_id;
        volatile int* finished;
    };

    auto increment_worker = [](void* arg) {
        thread_data* data = (thread_data*)arg;
        for(int i = 0; i < 1000; i++) {
            sem_wait(*data->mutex);
            (*data->counter)++;
            sem_signal(*data->mutex);

            if(i % 100 == 0) {
                thread_dispatch();
            }
        }
        data->finished[data->thread_id] = 1;
    };

    sem_open(&mutex, 1);  // Binary semaphore

    thread_t threads[3];
    thread_data data[3];

    for(int i = 0; i < 3; i++) {
        data[i].mutex = &mutex;
        data[i].counter = &shared_counter;
        data[i].thread_id = i;
        data[i].finished = thread_finished;
        thread_create(&threads[i], increment_worker, &data[i]);
    }

    // Wait for all threads to finish
    while(!thread_finished[0] || !thread_finished[1] || !thread_finished[2]) {
        thread_dispatch();
    }

    printString("Expected counter: 3000, Actual: ");
    printInt(shared_counter);
    if(shared_counter == 3000) {
        printString(" - PASSED\n");
    } else {
        printString(" - FAILED\n");
    }

    sem_close(mutex);
}

// ============================================================
// TEST 2: Multiple Producers, Multiple Consumers
// ============================================================
void test_multiple_consumers() {
    printString("\n=== Multiple Consumers Test ===\n");

    const int BUFFER_SIZE = 5;
    const int NUM_PRODUCERS = 2;
    const int NUM_CONSUMERS = 3;
    const int ITEMS_PER_PRODUCER = 10;

    sem_t empty, full, mutex_prod, mutex_cons;
    volatile int buffer[BUFFER_SIZE];
    volatile int in = 0, out = 0;
    volatile int total_consumed = 0;
    volatile int producer_done[NUM_PRODUCERS] = {0};
    volatile int consumer_done[NUM_CONSUMERS] = {0};

    struct producer_data {
        int id;
        sem_t *empty, *full, *mutex;
        volatile int* buffer;
        volatile int* in;
        volatile int* done;
    };

    struct consumer_data {
        int id;
        sem_t *empty, *full, *mutex;
        volatile int* buffer;
        volatile int* out;
        volatile int* total;
        volatile int* done;
        volatile int* all_producers_done;
    };

    auto producer_func = [](void* arg) {
        producer_data* data = (producer_data*)arg;
        for(int i = 0; i < ITEMS_PER_PRODUCER; i++) {
            int item = data->id * 100 + i;

            sem_wait(*data->empty);
            sem_wait(*data->mutex);

            data->buffer[*data->in] = item;
            *data->in = (*data->in + 1) % BUFFER_SIZE;

            sem_signal(*data->mutex);
            sem_signal(*data->full);

            if(i % 3 == 0) thread_dispatch();
        }
        data->done[data->id] = 1;
    };

    auto consumer_func = [](void* arg) {
        consumer_data* data = (consumer_data*)arg;
        int consumed = 0;

        while(1) {
            // Check if all producers are done
            int all_done = 1;
            for(int i = 0; i < NUM_PRODUCERS; i++) {
                if(!data->all_producers_done[i]) {
                    all_done = 0;
                    break;
                }
            }

            if(all_done && sem_trywait(*data->full) != 0) {
                break;  // No more items to consume
            }

            sem_wait(*data->mutex);

            //int item = data->buffer[*data->out];
            *data->out = (*data->out + 1) % BUFFER_SIZE;
            consumed++;

            sem_signal(*data->mutex);
            sem_signal(*data->empty);

            if(consumed % 2 == 0) thread_dispatch();
        }

        sem_wait(*data->mutex);
        *data->total += consumed;
        sem_signal(*data->mutex);

        data->done[data->id] = 1;
    };

    sem_open(&empty, BUFFER_SIZE);
    sem_open(&full, 0);
    sem_open(&mutex_prod, 1);
    sem_open(&mutex_cons, 1);

    thread_t producers[NUM_PRODUCERS];
    thread_t consumers[NUM_CONSUMERS];
    producer_data prod_data[NUM_PRODUCERS];
    consumer_data cons_data[NUM_CONSUMERS];

    // Create producers
    for(int i = 0; i < NUM_PRODUCERS; i++) {
        prod_data[i].id = i;
        prod_data[i].empty = &empty;
        prod_data[i].full = &full;
        prod_data[i].mutex = &mutex_prod;
        prod_data[i].buffer = buffer;
        prod_data[i].in = &in;
        prod_data[i].done = producer_done;
        thread_create(&producers[i], producer_func, &prod_data[i]);
    }

    // Create consumers
    for(int i = 0; i < NUM_CONSUMERS; i++) {
        cons_data[i].id = i;
        cons_data[i].empty = &empty;
        cons_data[i].full = &full;
        cons_data[i].mutex = &mutex_cons;
        cons_data[i].buffer = buffer;
        cons_data[i].out = &out;
        cons_data[i].total = &total_consumed;
        cons_data[i].done = consumer_done;
        cons_data[i].all_producers_done = producer_done;
        thread_create(&consumers[i], consumer_func, &cons_data[i]);
    }

    // Wait for completion
    while(1) {
        int all_done = 1;
        for(int i = 0; i < NUM_CONSUMERS; i++) {
            if(!consumer_done[i]) {
                all_done = 0;
                break;
            }
        }
        if(all_done) break;
        thread_dispatch();
    }

    printString("Total items produced: ");
    printInt(NUM_PRODUCERS * ITEMS_PER_PRODUCER);
    printString(", Total consumed: ");
    printInt(total_consumed);

    if(total_consumed == NUM_PRODUCERS * ITEMS_PER_PRODUCER) {
        printString(" - PASSED\n");
    } else {
        printString(" - FAILED\n");
    }

    sem_close(empty);
    sem_close(full);
    sem_close(mutex_prod);
    sem_close(mutex_cons);
}

// ============================================================
// TEST 3: Semaphore Stress Test
// ============================================================
void test_semaphore_stress() {
    printString("\n=== Semaphore Stress Test ===\n");

    const int NUM_THREADS = 10;
    const int ITERATIONS = 100;
    sem_t resource_pool;
    volatile int active_count = 0;
    volatile int max_active = 0;
    volatile int done_count = 0;
    sem_t count_mutex;

    struct worker_data {
        int id;
        sem_t* pool;
        sem_t* mutex;
        volatile int* active;
        volatile int* max;
        volatile int* done;
    };

    auto worker = [](void* arg) {
        worker_data* data = (worker_data*)arg;

        for(int i = 0; i < ITERATIONS; i++) {
            // Acquire resource
            sem_wait(*data->pool);

            // Update active count
            sem_wait(*data->mutex);
            (*data->active)++;
            if(*data->active > *data->max) {
                *data->max = *data->active;
            }
            sem_signal(*data->mutex);

            // Simulate work
            for(int j = 0; j < 10; j++) {
                if(j % 5 == 0) thread_dispatch();
            }

            // Update active count
            sem_wait(*data->mutex);
            (*data->active)--;
            sem_signal(*data->mutex);

            // Release resource
            sem_signal(*data->pool);

            if(i % 10 == 0) thread_dispatch();
        }

        sem_wait(*data->mutex);
        (*data->done)++;
        sem_signal(*data->mutex);
    };

    sem_open(&resource_pool, 3);  // Only 3 resources available
    sem_open(&count_mutex, 1);

    thread_t threads[NUM_THREADS];
    worker_data data[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
        data[i].id = i;
        data[i].pool = &resource_pool;
        data[i].mutex = &count_mutex;
        data[i].active = &active_count;
        data[i].max = &max_active;
        data[i].done = &done_count;
        thread_create(&threads[i], worker, &data[i]);
    }

    // Wait for all threads
    while(done_count < NUM_THREADS) {
        thread_dispatch();
    }

    printString("Max concurrent threads: ");
    printInt(max_active);
    if(max_active <= 3) {
        printString(" (should be <= 3) - PASSED\n");
    } else {
        printString(" (exceeded limit!) - FAILED\n");
    }

    sem_close(resource_pool);
    sem_close(count_mutex);
}

// ============================================================
// TEST 4: Deadlock Prevention Test
// ============================================================
void test_deadlock_prevention() {
    printString("\n=== Deadlock Prevention Test ===\n");

    sem_t sem1, sem2;
    volatile int completed[2] = {0, 0};
    volatile int deadlock_detected = 0;

    struct thread_data {
        int id;
        sem_t *first, *second;
        volatile int* completed;
    };

    auto worker = [](void* arg) {
        thread_data* data = (thread_data*)arg;

        for(int i = 0; i < 10; i++) {
            // Always acquire in the same order to prevent deadlock
            sem_wait(*data->first);
            sem_wait(*data->second);

            // Critical section
            for(int j = 0; j < 5; j++) {
                thread_dispatch();
            }

            sem_signal(*data->second);
            sem_signal(*data->first);

            thread_dispatch();
        }

        data->completed[data->id] = 1;
    };

    sem_open(&sem1, 1);
    sem_open(&sem2, 1);

    thread_t t1, t2;
    thread_data data1 = {0, &sem1, &sem2, completed};
    thread_data data2 = {1, &sem1, &sem2, completed};  // Same order!

    thread_create(&t1, worker, &data1);
    thread_create(&t2, worker, &data2);

    // Timeout mechanism
    int timeout = 0;
    while(!completed[0] || !completed[1]) {
        thread_dispatch();
        timeout++;
        if(timeout > 1000) {
            deadlock_detected = 1;
            break;
        }
    }

    if(deadlock_detected) {
        printString("Deadlock detected - FAILED\n");
    } else {
        printString("No deadlock - PASSED\n");
    }

    sem_close(sem1);
    sem_close(sem2);
}

// ============================================================
// TEST 5: Reader-Writer Problem
// ============================================================
void test_reader_writer() {
    printString("\n=== Reader-Writer Test ===\n");

    sem_t resource, mutex;
    volatile int reader_count = 0;
    volatile int shared_data = 0;
    volatile int readers_done = 0;
    volatile int writers_done = 0;

    struct reader_data {
        int id;
        sem_t *resource, *mutex;
        volatile int* reader_count;
        volatile int* data;
        volatile int* done;
    };

    struct writer_data {
        int id;
        sem_t* resource;
        volatile int* data;
        volatile int* done;
    };

    auto reader = [](void* arg) {
        reader_data* data = (reader_data*)arg;

        for(int i = 0; i < 5; i++) {
            // Entry section
            sem_wait(*data->mutex);
            (*data->reader_count)++;
            if(*data->reader_count == 1) {
                sem_wait(*data->resource);
            }
            sem_signal(*data->mutex);

            // Read data
            int value = *data->data;
            printString("Reader ");
            printInt(data->id);
            printString(" read: ");
            printInt(value);
            printString("\n");

            thread_dispatch();

            // Exit section
            sem_wait(*data->mutex);
            (*data->reader_count)--;
            if(*data->reader_count == 0) {
                sem_signal(*data->resource);
            }
            sem_signal(*data->mutex);

            thread_dispatch();
        }

        (*data->done)++;
    };

    auto writer = [](void* arg) {
        writer_data* data = (writer_data*)arg;

        for(int i = 0; i < 3; i++) {
            sem_wait(*data->resource);

            // Write data
            (*data->data)++;
            printString("Writer ");
            printInt(data->id);
            printString(" wrote: ");
            printInt(*data->data);
            printString("\n");

            sem_signal(*data->resource);

            thread_dispatch();
        }

        (*data->done)++;
    };

    sem_open(&resource, 1);
    sem_open(&mutex, 1);

    thread_t readers[3], writers[2];
    reader_data r_data[3];
    writer_data w_data[2];

    // Create readers
    for(int i = 0; i < 3; i++) {
        r_data[i].id = i;
        r_data[i].resource = &resource;
        r_data[i].mutex = &mutex;
        r_data[i].reader_count = &reader_count;
        r_data[i].data = &shared_data;
        r_data[i].done = &readers_done;
        thread_create(&readers[i], reader, &r_data[i]);
    }

    // Create writers
    for(int i = 0; i < 2; i++) {
        w_data[i].id = i;
        w_data[i].resource = &resource;
        w_data[i].data = &shared_data;
        w_data[i].done = &writers_done;
        thread_create(&writers[i], writer, &w_data[i]);
    }

    // Wait for completion
    while(readers_done < 3 || writers_done < 2) {
        thread_dispatch();
    }

    printString("Final value: ");
    printInt(shared_data);
    printString(" (expected: 6)\n");

    sem_close(resource);
    sem_close(mutex);
}

// Main test runner
void run_additional_semaphore_tests() {
    printString("\n========================================\n");
    printString("Running Additional Semaphore Tests\n");
    printString("========================================\n");

    test_binary_semaphore();
    test_multiple_consumers();
    test_semaphore_stress();
    test_deadlock_prevention();
    test_reader_writer();

    printString("\n========================================\n");
    printString("All Additional Tests Completed\n");
    printString("========================================\n");
}