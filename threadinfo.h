#ifndef THREADINFO_H
#define THREADINFO_H

#include <stdint.h>
#include <pthread.h>

typedef struct ThreadInfo {
   int fs;
   pthread_mutex_t* lock;
} ThreadInfo;

#endif
