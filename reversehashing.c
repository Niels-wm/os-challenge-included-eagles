#include "packet.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "threadinfo.h"
#include "hashtable.h"


// int threadPacketCount = 0;

// void *findAnswerPartial(void *arg) {
//   int *start = (int*)arg;
//   for (size_t i = start; i < (start + threadPacketCount); i++) {
//     /* code */
//   }
//
//   pthread_exit(NULL);
// }

void *reversehashing(void *arg) {
    ThreadInfo* ti = (ThreadInfo*)arg;
    uint8_t testHash[32];
    pthread_mutex_t* lock = (ti->lock);
    int fs = ti->fs;
    struct Packet packet;
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
    pthread_mutex_lock(lock);
    uint64_t foundAnswer = find(packet.hash);
    pthread_mutex_unlock(lock);
    // printf("\nFOUND value:  %" PRIu64, foundAnswer);

    bzero(testHash, 32);
    SHA256((char*) &foundAnswer, 8, testHash);

    // -- IMPLEMENT THE SCHEDULER HERE:

    // -- POP THE MOST IMPORTANT PACKET AND NEWSOCKFILEDESCRIPTER HERE:

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
      for (answer; answer <= packet.end; answer++){
        bzero(theHash, 32);
        unsigned char *hashedNumber = SHA256((char*) &answer, 8, theHash);

        if (memcmp(theHash, packet.hash, sizeof(theHash)) == 0) {
          printf("\nFound a match, with:  %" PRIu64, answer);
          break;
        }
      }
      pthread_mutex_lock(lock);
      insert(packet.hash, answer);
      pthread_mutex_unlock(lock);
      answer = htobe64(answer);
      n = write(fs, &answer, 8);

      if(n < 0) {
          perror("ERROR writing to socket");
          exit(1);
      }
    }
    free(ti);
    close(fs);
    pthread_exit(NULL);
}
