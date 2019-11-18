#include "packet.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "threadinfo.h"
#include "hashtable.h"

void reversehashing(void *tInfo, void *p) {

    //printf("started reversing\n");
    struct ThreadInfo* ti = (struct ThreadInfo*) tInfo;
    struct Packet* packetP = (struct Packet*) p;

    struct Packet packet = *packetP;
    //printf("packet %x \n", packet.start);
    

    //printf("TI: %d", ti->fs);
    uint8_t testHash[32];
    pthread_mutex_t* lock = (ti->lock);
    int fs = ti->fs;
    int i, n;

    //printf("part1 done\n");

    /* Receive */
    //bzero((char *)&packet, sizeof(packet));
    ////printf("part2\n");
    //n = read(ti->fs, &packet, sizeof(packet));

    ////printf("part3\n");
    //if (n < 0)
    //{
    //    perror("ERROR reading from socket");
    //    exit(1);
    //}

    // Reverse the start, end and p:
    packet.start = be64toh(packet.start);
    packet.end = be64toh(packet.end);


    //printf("hash: %x\n", packet.hash);
    // /* SHA 256 ALGO */
    uint64_t answer = packet.start;
    //printf("packet start: %d\n", packet.start);
    uint8_t theHash[32];
    //printf("prelock\n");
    // -- CHECK IF RECEIVED HASH IS A KNOWN HASH (IN HASHTABLE) AND SEND ANSWER TO CLIENT IF IT IS:
    pthread_mutex_lock(lock);
    uint64_t foundAnswer = find(packet.hash);
    pthread_mutex_unlock(lock);
    //printf("postlock\n");
    // //printf("\nFOUND value:  %" PRIu64, foundAnswer);

    bzero(testHash, 32);
    SHA256((char*) &foundAnswer, 8, testHash);


    if (foundAnswer != 0 && memcmp(testHash, packet.hash, sizeof(testHash)) == 0){
      //printf("\nFOUND in hashtable\n");
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
          ////printf("\nFound a match, with:  %" PRIu64, answer);
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

    close(fs);

    free(ti);
    
    return NULL;
}
