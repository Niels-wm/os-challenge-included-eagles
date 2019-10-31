#include "packet.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "threadinfo.h"
#include "hashtable.h"

pthread_mutex_t* hashTableLock;

void initReverseHashing(pthread_mutex_t* htLock){
  hashTableLock = htLock;
}

uint64_t reversehashing2(uint64_t start, uint64_t end, uint8_t *hash){
  uint64_t answer;
  uint8_t testHash[32];
  start = be64toh(start);
  end = be64toh(end);
  printf("START %"PRIu64, start);
  printf("END %"PRIu64, end);

  for (answer = start; answer <= end; answer++){
    bzero(testHash, 32);
    // printf("ANSWER: %"PRIu64, answer);
    // printf("\n" );
    SHA256((char*) &answer, 8, testHash);
    if (memcmp(testHash, hash, sizeof(testHash)) == 0) {
      printf("\nFIND ANYTHING\n" );
      return htobe64(answer);
    }
  }
  printf("\nDIDN'T FIND ANYTHING\n" );
}

void *reversehashing(void *arg) {

    struct Packet packet;
    struct ThreadInfo *ti = (struct ThreadInfo*) arg;

    uint8_t testHash[32];
    int fs = ti->fs;
    int i, n;

    /* Receive */
    bzero((char *)&packet, sizeof(packet));
    n = read(ti->fs, &packet, sizeof(packet));

    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }

    // Reverse the start, end and p:
    packet.start = be64toh(packet.start);
    packet.end = be64toh(packet.end);

    // /* SHA 256 ALGO */
    uint64_t answer = packet.start;
    uint8_t theHash[32];

    // -- CHECK IF RECEIVED HASH IS A KNOWN HASH (IN HASHTABLE) AND SEND ANSWER TO CLIENT IF IT IS:
    pthread_mutex_lock(hashTableLock);
    uint64_t foundAnswer = find(packet.hash);
    pthread_mutex_unlock(hashTableLock);
    // printf("\nFOUND value:  %" PRIu64, foundAnswer);

    bzero(testHash, 32);
    SHA256((char*) &foundAnswer, 8, testHash);


    if (foundAnswer != 0 && memcmp(testHash, packet.hash, sizeof(testHash)) == 0){
      printf("\nFOUND in hashtable\n");
      foundAnswer = be64toh(foundAnswer);
      n = write(fs, &foundAnswer, 8);

      if(n < 0) {
          perror("ERROR writing to socket");
          exit(1);
      }
    // If no value found in hash table use brute force algorithm
    }
    else {
      uint64_t answer = reversehashing2(packet.start, packet.end, packet.hash);
      // for (answer; answer <= packet.end; answer++){
      //   bzero(theHash, 32);
      //   unsigned char *hashedNumber = SHA256((char*) &answer, 8, theHash);
      //
      //   if (memcmp(theHash, packet.hash, sizeof(theHash)) == 0) {
      //     //printf("\nFound a match, with:  %" PRIu64, answer);
      //     break;
      //   }
      // }
      pthread_mutex_lock(hashTableLock);
      insert(packet.hash, answer);
      pthread_mutex_unlock(hashTableLock);

      // answer = htobe64(answer);
      n = write(fs, &answer, 8);

      if(n < 0) {
          perror("ERROR writing to socket");
          exit(1);
      }
    }

    close(fs);

    free(ti);
    pthread_exit(NULL);
    return NULL;
}
