#ifndef THREADINFO_H
#define THREADINFO_H

#include <stdint.h>
#include <pthread.h>

struct ThreadInfo {
   int fs;
   pthread_mutex_t* lock;
};

#endif
