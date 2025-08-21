//
// Created by os on 7/11/25.
//

#include "../h/MemoryAllocator.hpp"

MemBlock* MemoryAllocator::freeMem = nullptr;
MemBlock* MemoryAllocator::usedMem = nullptr;

void MemoryAllocator::initialize() {
    freeMem = (MemBlock*)HEAP_START_ADDR;
    size_t heapSize = ((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR) - sizeof(MemBlock);
    freeMem->size = heapSize / MEM_BLOCK_SIZE;
    freeMem->next = nullptr;
    usedMem = nullptr;
}

void* MemoryAllocator::mem_alloc(size_t size) {
    if (size == 0) return nullptr;

    size_t blocks = (size + sizeof(MemBlock) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    MemBlock* prev = nullptr;
    MemBlock* curr = freeMem;

    while (curr) {
        if (curr->size >= blocks) {
            if (curr->size > blocks) {
                MemBlock* newBlock = (MemBlock*)((char*)curr + blocks * MEM_BLOCK_SIZE);
                newBlock->size = curr->size - blocks;
                newBlock->next = curr->next;

                if (prev) {
                    prev->next = newBlock;
                } else {
                    freeMem = newBlock;
                }
            } else {
                if (prev) {
                    prev->next = curr->next;
                } else {
                    freeMem = curr->next;
                }
            }

            curr->size = blocks;
            curr->next = usedMem;
            usedMem = curr;

            return (char*)curr + sizeof(MemBlock);
        }

        prev = curr;
        curr = curr->next;
    }

    return nullptr;
}

int MemoryAllocator::mem_free(void* ptr) {
    if (!ptr) return 0;

    if (!isValidHeapAddress(ptr)) return -1;

    MemBlock* block = (MemBlock*)((char*)ptr - sizeof(MemBlock));

    MemBlock* prev = nullptr;
    MemBlock* curr = usedMem;

    while (curr) {
        if (curr == block) {
            if (prev) {
                prev->next = curr->next;
            } else {
                usedMem = curr->next;
            }

            insertToFreeList(block);
            coalesceBlocks();

            return 0;
        }
        prev = curr;
        curr = curr->next;
    }

    return -2;
}

//MemBlock* MemoryAllocator::findFirstFit(size_t blocks) {
//    MemBlock* curr = freeMem;
//    while (curr) {
//        if (curr->size >= blocks) {
//            return curr;
//        }
//        curr = curr->next;
//    }
//    return nullptr;
//}

void MemoryAllocator::insertToFreeList(MemBlock* block) {
    if (!freeMem || block < freeMem) {
        block->next = freeMem;
        freeMem = block;
        return;
    }

    MemBlock* prev = freeMem;
    MemBlock* curr = freeMem->next;

    while (curr && curr < block) {
        prev = curr;
        curr = curr->next;
    }

    prev->next = block;
    block->next = curr;
}

void MemoryAllocator::coalesceBlocks() {
    if (!freeMem) return;

    MemBlock* curr = freeMem;

    while (curr && curr->next) {
        MemBlock* nextBlock = (MemBlock*)((char*)curr + curr->size * MEM_BLOCK_SIZE);

        if (nextBlock == curr->next) {
            curr->size += curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

bool MemoryAllocator::isValidHeapAddress(void* addr) {
    return addr >= HEAP_START_ADDR && addr < HEAP_END_ADDR;
}

//void MemoryAllocator::printHeapStatus() {
//    size_t freeBlocks = 0;
//    size_t usedBlocks = 0;
//    size_t freeCount = 0;
//    size_t usedCount = 0;
//
//    MemBlock* curr = freeMem;
//    while (curr) {
//        freeBlocks += curr->size;
//        freeCount++;
//        curr = curr->next;
//    }
//
//    curr = usedMem;
//    while (curr) {
//        usedBlocks += curr->size;
//        usedCount++;
//        curr = curr->next;
//    }
//}
//
//size_t MemoryAllocator::getUsedMemory() {
//    size_t usedBlocks = 0;
//    MemBlock* curr = usedMem;
//    while (curr) {
//        usedBlocks += curr->size;
//        curr = curr->next;
//    }
//    return usedBlocks * MEM_BLOCK_SIZE;
//}
//
//size_t MemoryAllocator::getFreeMemory() {
//    size_t freeBlocks = 0;
//    MemBlock* curr = freeMem;
//    while (curr) {
//        freeBlocks += curr->size;
//        curr = curr->next;
//    }
//    return freeBlocks * MEM_BLOCK_SIZE;
//}