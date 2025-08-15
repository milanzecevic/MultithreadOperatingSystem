//
// Created by os on 8/15/25.
//

#ifndef OSPROJECT_SEM_HPP
#define OSPROJECT_SEM_HPP

#include "tcb.hpp"
#include "list.hpp"
#include "scheduler.hpp"

class Semaphore {
public:
    Semaphore(unsigned init=1) : val(init) {}
    void sem_close();

    void sem_wait();
    void sem_signal();

    int sem_trywait();

    static Semaphore *createSem(uint64 val);

    int value() const { return val;}

protected:
    void block();
    void unblock();

private:
    int val;
    List<TCB> blockedThreadQueue;
};


#endif //OSPROJECT_SEM_HPP
