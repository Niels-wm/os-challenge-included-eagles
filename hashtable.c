#include <stdint.h>
#include "messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

pthread_mutex_t hashTableLock;

#define SIZE 256
#define HASH_SIZE 32
uint64_t hashArray[SIZE] = {0};

void initHashTable() {
  pthread_mutex_init(&hashTableLock, NULL);
}

uint8_t hashIndex(const uint8_t *key) {
    uint8_t index = 0;

    int i;
    for (i = 0; i < HASH_SIZE; ++i) {
        index = (index +  (key[i]^index)) ;//% SIZE;
    }

    return index;
}

uint64_t find(uint8_t *key) {
  return hashArray[hashIndex(key)];
}

void insert(uint8_t *key, const uint64_t value) {
    int hashKey = hashIndex(key);
    pthread_mutex_lock(&hashTableLock);
    hashArray[hashKey] = value;
    pthread_mutex_unlock(&hashTableLock);

}
