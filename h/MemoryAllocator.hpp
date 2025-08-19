//
// Created by os on 7/11/25.
//

#ifndef OSPROJECT_MEMORYALLOCATOR_HPP
#define OSPROJECT_MEMORYALLOCATOR_HPP
#include "../lib/hw.h"

struct MemBlock{
    size_t size;
    MemBlock *next;
};

class MemoryAllocator {
private:
    static MemBlock *freeMem;
    static MemBlock *usedMem;

    static MemBlock* findFirstFit(size_t blocks);
    static void insertToFreeList(MemBlock* block);
    static void coalesceBlocks();
    static bool isValidHeapAddress(void* addr);

public:
    static void initialize();
    static void* mem_alloc(size_t size);
    static int mem_free(void* ptr);

    static void printHeapStatus();
    static size_t getUsedMemory();
    static size_t getFreeMemory();
};


#endif //OSPROJECT_MEMORYALLOCATOR_HPP
