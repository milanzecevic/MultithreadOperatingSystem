//
// Created by os on 7/20/25.
//

#ifndef OSPROJECT_TCB_HPP
#define OSPROJECT_TCB_HPP


#include "../lib/hw.h"
#include "scheduler.hpp"
#include "syscall_c.hpp"

// Thread Control Block
class TCB
{
public:
    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    using Body = void (*)();

    static TCB *createThread(Body body, void* arg, char* stack_space);

    static TCB *running;

private:
    TCB(Body body, void* arg, char* stack_space) :
            body(body),
            arg(arg),
            stack(stack_space),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            finished(false)
    {
        if (body != nullptr) { Scheduler::put(this); }
    }

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* arg;
    char* stack;
    Context context;
    bool finished;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

};


#endif //OSPROJECT_TCB_HPP
