#include "Packet.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reversehashing (struct Packet packet1, int sock) {
    int i, n;

    printf("Received hash:\n");
    for (i = 0; i < 32; i++){
        printf("%0x", packet1.hash[i]);
    }

    /* SHA 256 ALGO */ 
    uint64_t answer = packet1.start;
    uint8_t theHash[32];

    for (answer; answer <= packet1.end; answer++){

        bzero(theHash, 32);
        unsigned char *hashedNumber = SHA256((char*) &answer, 8, theHash);


        if (memcmp(theHash, packet1.hash, sizeof(theHash)) == 0) {
            printf("\nFound a match, with:  %" PRIu64, answer);
            break;
        }
    }

    
    printf("\nCalculated hash:\n");
    for (i = 0; i < 32; i++){
        printf("%0x", theHash[i]);
    }
    printf("\n");


    /* Send */
    answer = htobe64(answer);
    n = write(sock, &answer, 8);

    if(n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}