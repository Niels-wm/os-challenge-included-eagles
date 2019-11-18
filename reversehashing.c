#include "structs.h"
#include "messages.h"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


void reversehashing (struct Request request) {
    struct Packet packet1 = request.packet;
    int n;
    int sock = request.reply_socket;
    
    // Reverse the start, end and p:
    packet1.start = be64toh(packet1.start);
    packet1.end = be64toh(packet1.end);


    /* SHA 256 ALGO */ 
    uint64_t answer;
    uint8_t theHash[32];

    for (answer = packet1.start; answer <= packet1.end; answer++){

        bzero(theHash, 32);
        SHA256((const unsigned char *) &answer, 8, theHash);

        if (testcmp(theHash, packet1.hash, sizeof(theHash)) == 0) {
            break;
        }
    }

    /* Send */
    answer = htobe64(answer);
    n = write(sock, &answer ,8);
    close(sock);

    if(n < 0) {
        perror("ERROR writing to socket");
    }
}



// Optimization of memcm - Lasse is the author of this section:
// Main source: https://macosxfilerecovery.com/faster-memory-comparison-in-c/
int testcmp(const unsigned char *hash1, const unsigned char *hash2, unsigned int length) {
  if (length >= 4) { // Check if value is aligned in a 4-byte boundary.
    int diff = *(int *)hash1 - *(int *)hash2;
    if (diff)
      return diff;
  }
  return memcmp(hash1, hash2, length);
}
