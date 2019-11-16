#include "structs.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <unistd.h>

#include "hashtable.h"

int testcmp(const unsigned char *hash1, const unsigned char *hash2, unsigned int length);

void *reversehashing(struct Request request, pthread_mutex_t* lock) {

    struct Packet packet = request.packet;

    uint8_t testHash[32];
    int sock = request.reply_socket;
    int i, n;

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
    SHA256((const unsigned char*) &foundAnswer, 8, testHash);
    if (foundAnswer != 0 && testcmp(testHash, packet.hash, sizeof(testHash)) == 0){
        printf("\nFOUND in hashtable\n");
        foundAnswer = be64toh(foundAnswer);
        n = write(sock, &foundAnswer, 8);

        if(n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
        // If no value found in hash table use brute force algorithm
    }
    else {
        for (answer; answer <= packet.end; answer++){
            bzero(theHash, 32);
            SHA256((const unsigned char*) &answer, 8, theHash);

            if (testcmp(theHash, packet.hash, sizeof(theHash)) == 0) {
                //printf("\nFound a match, with:  %" PRIu64, answer);
                break;
            }
        }
        pthread_mutex_lock(lock);
        insert(packet.hash, answer);
        pthread_mutex_unlock(lock);

        answer = htobe64(answer);
        n = write(sock, &answer, 8);

        if(n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }

    close(sock);

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
