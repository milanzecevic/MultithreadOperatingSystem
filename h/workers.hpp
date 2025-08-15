//
// Created by os on 8/14/25.
//

#ifndef OSPROJECT_WORKERS_HPP
#define OSPROJECT_WORKERS_HPP


#include "../lib/hw.h"
#include "tcb.hpp"
#include "print.hpp"

extern void workerBodyA(void *arg);

extern void workerBodyB(void *arg);

extern void workerBodyC(void *arg);

extern void workerBodyD(void *arg);


#endif //OSPROJECT_WORKERS_HPP
