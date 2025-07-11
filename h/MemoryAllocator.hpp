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
    static MemBlock *freeMem;
    static MemBlock *usedMem;
public:
    static void initialize();
    static void* mem_alloc (size_t size);
    static int mem_free (void*);
};


#endif //OSPROJECT_MEMORYALLOCATOR_HPP
