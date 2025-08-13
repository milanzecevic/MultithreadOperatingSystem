//
// Created by os on 7/19/25.
//

#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../lib/mem.h"   //dok ne uvezem svoj MemmoryAllocator


void Riscv::popSppSpie() {
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap() {
    uint64 scause = r_scause();
    if(scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        uint64 sysCallCode;
        __asm__ volatile("mv %0, a0" : "=r"(sysCallCode));
        switch (sysCallCode) {
            case 0x01: { //mem_alloc
                size_t size;
                __asm__ volatile("mv %0, a1" : "=r"(size));
                void* returnAdr = __mem_alloc(size);
                __asm__ volatile("sd %0, 80(s0)" : : "r"(returnAdr));
                break;
            }
            case 0x02: { //mem_free
                void* adrPtr;
                __asm__ volatile("mv %0, a1" : "=r"(adrPtr));
                int retVal = __mem_free(adrPtr);
                __asm__ volatile("sd %0, 80(s0)" : : "r"(retVal));
                break;
            }
            case 0x11: { //thread_create
                TCB** handle;
                TCB::Body body;
                void* arg;
                char* stack_space;
                __asm__ volatile("mv %0, a1" : "=r"(handle));
                __asm__ volatile("mv %0, a2" : "=r"(body));
                __asm__ volatile("mv %0, a3" : "=r"(arg));
                __asm__ volatile("mv %0, a4" : "=r"(stack_space));
                *handle = TCB::createThread(body, arg, stack_space);
                int retVal = (*handle != nullptr) ? 0 : -1;
                __asm__ volatile("sd %0, 80(s0)" : : "r"(retVal));
                break;
            }
            case 0x12: { //thread_exit
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
                break;
            }
            case 0x22: { //sem_close
                break;
            }
            case 0x23: { //sem_wait
                break;
            }
            case 0x24: { //sem_signal
                break;
            }
            case 0x26: { //sem_trywait
                break;
            }
//            case 0x41: { //getc
//                break;
//            }
//            case 0x42: { //putc
//                break;
//            }
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    } else if(scause == 0x8000000000000009UL) {
        console_handler();
    } else {
        //error
        printString("Error!!!");

        volatile int* shutdownAdr = (int*) 0x100000;
        *shutdownAdr = 0x5555;
    }
}


