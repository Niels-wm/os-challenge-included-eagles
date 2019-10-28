
#ifndef PACKET_H
#define PACKET_H

#include <inttypes.h>

struct Packet {
   uint8_t hash[32];
   uint64_t start;
   uint64_t end;
   uint8_t p;
   int id;

};

#endif
