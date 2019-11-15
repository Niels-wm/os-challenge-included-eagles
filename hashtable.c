#include <stdint.h>
#include "messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "hashpacket.h"

pthread_mutex_t hashTableLock;

#define MULTIPLIER 37
#define SIZE 4096
#define HASH_SIZE 32
struct HashPacket hashArray[SIZE] = {};
int counter = 0;

void initHashTable() {
  pthread_mutex_init(&hashTableLock, NULL);
}

unsigned long hashIndex(uint8_t key[32]) {
    unsigned long index = 0;
    int i;

    for (i = 0; i < HASH_SIZE; ++i) {
        index = (index * MULTIPLIER + key[i]);
    }
    return index % SIZE;
}

uint64_t find(uint8_t key[32]) {
  int hashKey = hashIndex(key);

  while(hashArray[hashKey].value != 0) {
    if (memcmp(hashArray[hashKey].key, key, HASH_SIZE*sizeof(uint8_t)) == 0){
      printf("FOUND\n" );
      return hashArray[hashKey].value;
    }
    printf("LOOKING\n");
    hashKey = (hashKey + 1) % SIZE;
  }

  return 0;
}

void insert(uint8_t key[], const uint64_t value) {
    int hashKey = hashIndex(key);

    while(hashArray[hashKey].value != 0) {
      if (value == hashArray[hashKey].value){
        printf("VALUE ALREADY STORED\n");
        return;
      }
      printf("COLLISSION\n" );
      hashKey = (hashKey + 1) % SIZE;
    }

    pthread_mutex_lock(&hashTableLock);
    counter++;
    hashArray[hashKey].value = value;
    memcpy(hashArray[hashKey].key, key, HASH_SIZE*sizeof(uint8_t));
    pthread_mutex_unlock(&hashTableLock);
    printf("AMOUNT IN HASHTABLE %d\n", counter);

}
