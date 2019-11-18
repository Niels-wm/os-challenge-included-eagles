
#ifndef STRUCTS_H
#define STRUCTS_H

#include <inttypes.h>

struct Packet {
   uint8_t hash[32];
   uint64_t start;
   uint64_t end;
   uint8_t p;
};

struct Request {
    struct Packet packet;
    uint64_t prio;
    int reply_socket;

#endif
