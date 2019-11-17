#include "packet.h"
#include "messages.h"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "threadinfo.h"
#include "hashtable.h"

int testcmp(const unsigned char *hash1, const unsigned char *hash2, unsigned int length);

void *reversehashing(void *arg) {

    struct Packet packet;
    struct ThreadInfo *ti = (struct ThreadInfo*) arg;

    // uint8_t testHash[32];
    int fs = ti->fs;
    int n;

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
    uint64_t foundAnswer = find(packet.hash);
    // printf("\nFOUND value:  %" PRIu64, foundAnswer);

    // bzero(testHash, 32);
    // SHA256((const unsigned char*) &foundAnswer, 8, testHash);



    if (foundAnswer != 0){
      foundAnswer = be64toh(foundAnswer);
      n = write(fs, &foundAnswer, 8);

      if(n < 0) {
          perror("ERROR writing to socket");
          exit(1);
      }
    // If no value found in hash table use brute force algorithm
    }
    else {
      for (answer = packet.start; answer <= packet.end; answer++){
        bzero(theHash, 32);
        SHA256((const unsigned char*) &answer, 8, theHash);

        if (testcmp(theHash, packet.hash, sizeof(theHash)) == 0) {
          //printf("\nFound a match, with:  %" PRIu64, answer);
          break;
        }
      }

      insert(packet.hash, answer);

      answer = htobe64(answer);
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

// Optimization of memcmp
// Main source: https://macosxfilerecovery.com/faster-memory-comparison-in-c/
int testcmp(const unsigned char *hash1, const unsigned char *hash2, unsigned int length) {
  if (length >= 4) { // Check if value is aligned in a 4-byte boundary.
    int diff = *(int *)hash1 - *(int *)hash2;
    if (diff)
      return diff;
  }
  return memcmp(hash1, hash2, length);
}
