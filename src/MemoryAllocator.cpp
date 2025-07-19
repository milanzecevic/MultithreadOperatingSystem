//
// Created by os on 7/11/25.
//

#include "../h/MemoryAllocator.hpp"
MemBlock* MemoryAllocator::freeMem = nullptr;
MemBlock* MemoryAllocator::usedMem = nullptr;