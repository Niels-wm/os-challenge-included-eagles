#ifndef HASHPACKET_H
#define HASHPACKET_H

#include <stdint.h>

struct HashPacket {
   uint8_t key[32];
   uint64_t value;
};

#endif
