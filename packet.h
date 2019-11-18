
#ifndef PACKET_H
#define PACKET_H

#include <inttypes.h>
#include <stdbool.h>
#include <pthread.h>


struct Packet {
   uint8_t hash[32];
   uint64_t start;
   uint64_t end;
   uint8_t p;
};

struct arg_struct {
    int fileDescripter;
    struct Packet packet;
    bool* running;
    pthread_mutex_t* lock_running;
    int id;
    pthread_t main_thread;
};

#endif
