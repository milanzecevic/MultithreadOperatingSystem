//
// Created by os on 7/20/25.
//
//     ------DEO KODA PREUZET SA VEZBI 7------
//     ------ZADATAK 06 I ZADATAK 07------

#ifndef OSPROJECT_TCB_HPP
#define OSPROJECT_TCB_HPP


#include "../lib/hw.h"
#include "scheduler.hpp"
#include "syscall_c.h"

// Thread Control Block
class TCB
{
public:
//    enum State { READY, RUNNING, BLOCKED, FINISHED };

    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    bool isBlocked() const { return blocked; }

    void setBlocked(bool blocked) { blocked = blocked; }

//    State getState() const { return state; }
//
//    void setState(State s) { state = s; }

    using Body = void (*)(void *);

    static TCB *createThread(Body body, void* arg, char* stack_space);

    static TCB *running;

private:
    TCB(Body body, void* arg, char* stack_space) :
            body(body),
            arg(arg),
            stack(body != nullptr ? stack_space : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            finished(false),
            blocked(false)
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
    //State state;
    bool finished;
    bool blocked;

    friend class Riscv;
    friend class Semaphore;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

};


#endif //OSPROJECT_TCB_HPP
