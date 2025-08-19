//
// Created by os on 7/19/25.
//

#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/tcb.hpp"
#include "../h/sem.hpp"
#include "../h/print.hpp"
//#include "../lib/mem.h"   //dok ne uvezem svoj MemmoryAllocator
#include "../h/MemoryAllocator.hpp"

void Riscv::popSppSpie() {
    mc_sstatus(SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap() {
    uint64 scause = r_scause();
    if(scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        uint64 sysCallCode;
        __asm__ volatile("ld %0, 80(s0)" : "=r"(sysCallCode));
        switch (sysCallCode) {
            case 0x01: { //mem_alloc
                size_t size;
                __asm__ volatile("ld %0, 88(s0)" : "=r"(size));
                size = size * MEM_BLOCK_SIZE;
//                void* returnAdr = __mem_alloc(size);
                void* returnAdr = MemoryAllocator::mem_alloc(size);
                __asm__ volatile("sd %0, 80(s0)" : : "r"(returnAdr));
                break;
            }
            case 0x02: { //mem_free
                void* adrPtr;
                __asm__ volatile("ld %0, 88(s0)" : "=r"(adrPtr));
//                int retVal = __mem_free(adrPtr);
                int retVal = MemoryAllocator::mem_free(adrPtr);
                __asm__ volatile("sd %0, 80(s0)" : : "r"(retVal));
                break;
            }
            case 0x11: { //thread_create
                TCB** handle;
                TCB::Body body;
                void* arg;
                char* stack_space;
                __asm__ volatile("ld %0, 88(s0)" : "=r"(handle));
                __asm__ volatile("ld %0, 96(s0)" : "=r"(body));
                __asm__ volatile("ld %0, 104(s0)" : "=r"(arg));
                __asm__ volatile("ld %0, 112(s0)" : "=r"(stack_space));
                *handle = TCB::createThread(body, arg, stack_space);
                int retVal = (*handle != nullptr) ? 0 : -1;
                __asm__ volatile("sd %0, 80(s0)" : : "r"(retVal));
                break;
            }
            case 0x12: { //thread_exit
                //TCB::running->setState(TCB::FINISHED);
                TCB::running->setFinished(true);
                TCB::dispatch();
                __asm__ volatile("sd %0, 80(s0)" : : "r"(0));
                break;
            }
            case 0x13: { //thread_dispatch
                TCB::dispatch();
                break;
            }
            case 0x21: { //sem_open
                Semaphore** handle;
                uint64 init;
                __asm__ volatile("ld %0, 88(s0)" : "=r"(handle));
                __asm__ volatile("ld %0, 96(s0)" : "=r"(init));
                *handle = Semaphore::createSem(init);
                int retVal = (*handle != nullptr) ? 0 : -1;
                __asm__ volatile("sd %0, 80(s0)" : : "r"(retVal));
                break;
            }
            case 0x22: { //sem_close
                Semaphore* handle;
                __asm__ volatile("ld %0, 88(s0)" : "=r"(handle));
                handle->sem_close();
                __asm__ volatile("sd %0, 80(s0)" : : "r"(0));
                break;
            }
            case 0x23: { //sem_wait
                Semaphore* handle;
                __asm__ volatile("ld %0, 88(s0)" : "=r"(handle));
                handle->sem_wait();
                __asm__ volatile("sd %0, 80(s0)" : : "r"(0));
                break;
            }
            case 0x24: { //sem_signal
                Semaphore* handle;
                __asm__ volatile("ld %0, 88(s0)" : "=r"(handle));
                handle->sem_signal();
                __asm__ volatile("sd %0, 80(s0)" : : "r"(0));
                break;
            }
            case 0x26: { //sem_trywait
                Semaphore* handle;
                int retVal = -1;
                __asm__ volatile("ld %0, 88(s0)" : "=r"(handle));
                retVal = handle->sem_trywait();
                __asm__ volatile("sd %0, 80(s0)" : : "r"(retVal));
                break;
            }
            case 0x41: { //getc
                char c = __getc();
                __asm__ volatile("sd %0, 80(s0)" : : "r"(c));
                break;
            }
            case 0x42: { //putc
                char c;
                __asm__ volatile("ld %0, 88(s0)" : "=r"(c));
                __putc(c);
                break;
            }
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    } else if(scause == 0x8000000000000009UL) {
        console_handler();
    } else if(scause == 0x8000000000000001UL) {
        Riscv::mc_sip(SIP_SSIE);
    } else {
        //error
        printStringMoj("Error!!!");
        printStringMoj("\n");
        printStringMoj("SEPC: ");
        uint64 volatile sepc = r_sepc();
        printIntMoj(sepc);
        printStringMoj("\n");

        printStringMoj("SCAUSE: ");
        uint64 volatile scause = r_scause();
        printIntMoj(scause);
        printStringMoj("\n");

        volatile int* shutdownAdr = (int*) 0x100000;
        *shutdownAdr = 0x5555;
    }
}


