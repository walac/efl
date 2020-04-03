#ifndef PTHREAD_H
#define PTHREAD_H

#include "unimplemented.h"

#include "unposix_api.h"

UNIMPLEMENTED typedef unsigned long long pthread_t;

UNIMPLEMENTED_STRUCT_T(pthread_attr)

UNIMPLEMENTED UNPOSIX_API struct sched_param {
    int sched_priority;
};

#define SCHED_RR 5
#define SCHED_FIFO 6

#define PTHREAD_CANCEL_ASYNCHRONOUS 2
#define PTHREAD_CANCEL_ENABLE 3
#define PTHREAD_CANCEL_DEFERRED 4
#define PTHREAD_CANCEL_DISABLE 5
#define PTHREAD_CANCELED 6
#define PTHREAD_CREATE_DETACHED 7
#define PTHREAD_CREATE_JOINABLE 8
#define PTHREAD_EXPLICIT_SCHED 9
#define PTHREAD_INHERIT_SCHED 10
#define PTHREAD_ONCE_INIT 17
#define PTHREAD_PRIO_INHERIT 18
#define PTHREAD_PRIO_NONE 19
#define PTHREAD_PRIO_PROTECT 20
#define PTHREAD_PROCESS_SHARED 21
#define PTHREAD_PROCESS_PRIVATE 22
#define PTHREAD_SCOPE_PROCESS 23
#define PTHREAD_SCOPE_SYSTEM 24

#endif
