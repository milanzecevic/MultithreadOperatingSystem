//
// Created by os on 7/19/25.
//
//     ------KOD PREUZET SA VEZBI 7------
//     ------ZADATAK 06 I ZADATAK 07------

#include "../h/scheduler.hpp"

List<TCB> Scheduler::readyThreadQueue;

TCB *Scheduler::get()
{
    return readyThreadQueue.removeFirst();
}

void Scheduler::put(TCB *ccb)
{
    readyThreadQueue.addLast(ccb);
}