#include <stdint.h>
#include "messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 8224
#define HASH_SIZE 32
uint64_t hashArray[SIZE] = {0};

uint8_t hashIndex(const uint8_t *key) {
    int index = 0;

    int i;
    for (i = 0; i < HASH_SIZE; ++i) {
        index += (key[i]^index) %8224;
    }

    return index % 8224;
}

uint64_t find(uint8_t *key) {
  return hashArray[hashIndex(key)];
}

void insert(uint8_t *key, const uint64_t value) {
    int hashKey = hashIndex(key);
    hashArray[hashKey] = value;
}
