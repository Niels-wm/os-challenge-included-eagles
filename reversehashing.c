#include "packet.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "threadinfo.h"
#include "hashtable.h"

void *reversehashing(void *arg) {

    struct Packet packet;
    struct ThreadInfo *ti = (struct ThreadInfo*) arg;


    uint8_t testHash[32];
    pthread_mutex_t* lock = (ti->lock);
    int fs = ti->fs;
    int i, n;
	
	//Seed for the rand() generated by the time initialization
	//srand(time(0));

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
	uint64_t start = packet.start;
    uint64_t answer = start;
    uint8_t theHash[32];

    // -- CHECK IF RECEIVED HASH IS A KNOWN HASH (IN HASHTABLE) AND SEND ANSWER TO CLIENT IF IT IS:
    pthread_mutex_lock(lock);
    uint64_t foundAnswer = find(packet.hash);
    pthread_mutex_unlock(lock);
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
		//First itteration of the hashed algorithm, simplest possible version without a seed
		
		printf("\nStart:  %" PRIu64, start);
		printf("\nEnd:  %" PRIu64, sizeof(theHash));
		while (!(memcmp(theHash, packet.hash, sizeof(theHash)) == 0)) {
			bzero(theHash, 32);
			answer = (rand() % (sizeof(theHash) - start + 1)) + start;
			printf("\nRandomly Generated:  %" PRIu64, answer);
			unsigned char *hashedNumber = SHA256((char*) &answer, 8, theHash);
		}
      /*for (answer; answer <= packet.end; answer++){
        bzero(theHash, 32);
        unsigned char *hashedNumber = SHA256((char*) &answer, 8, theHash);

        if (memcmp(theHash, packet.hash, sizeof(theHash)) == 0) {
          //printf("\nFound a match, with:  %" PRIu64, answer);
          break;
        }
      } */
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
    pthread_exit(NULL);
    return NULL;
}
