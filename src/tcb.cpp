//
// Created by os on 7/20/25.
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"

TCB *TCB::running = nullptr;

TCB *TCB::createThread(Body body, void* arg, char* stack_space)
{
    return new TCB(body, arg, stack_space);
}

void TCB::yield()
{
    __asm__ volatile ("ecall");
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body();
    running->setFinished(true);
    TCB::yield();
}

