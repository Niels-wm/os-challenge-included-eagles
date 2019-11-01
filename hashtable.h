#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>

void initHashTable();
void insert(uint8_t *key, uint64_t value);
uint64_t find(uint8_t *key);

typedef struct HashInfo {
   uint8_t hash[32];
   uint64_t value;
} HashInfo;

#endif //HASHTABLE_H
// comment
