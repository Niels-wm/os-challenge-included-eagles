#include "packet.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "threadinfo.h"
#include "hashtable.h"

pthread_mutex_t hashTableLock;

void initReverseHashing(){
  pthread_mutex_init(&hashTableLock, NULL);
}

uint64_t reversehashing(uint64_t start, uint64_t end, uint8_t *hash){
  uint64_t answer;
  uint8_t testHash[32];

  bzero(testHash, 32);

  pthread_mutex_lock(&hashTableLock);
  answer = find(hash);
  pthread_mutex_unlock(&hashTableLock);

  if(answer != 0) {
    printf("FOUND1\n");
  }

  SHA256((char*) &answer, 8, testHash);
  if (answer != 0 && memcmp(testHash, hash, sizeof(testHash)) == 0){
    printf("FOUND2 \n");
    return be64toh(answer);
  }

  start = be64toh(start);
  end = be64toh(end);

  for (answer = start; answer <= end; answer++){
    SHA256((char*) &answer, 8, testHash);
    if (memcmp(testHash, hash, sizeof(testHash)) == 0) {
      pthread_mutex_lock(&hashTableLock);
      insert(hash, answer);
      pthread_mutex_unlock(&hashTableLock);
      return htobe64(answer);
    }
  }
}
