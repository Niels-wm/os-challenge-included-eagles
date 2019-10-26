#include "packet.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct Packet packet;
pthread_mutex_t lock;

void *reversehashing(void *arg) {
    int i, n;
    int newSockFileDescripter = *((int *)arg);
    pthread_mutex_lock(&lock);

    /* Receive */
    bzero((char *)&packet, sizeof(packet));
    n = read(newSockFileDescripter, &packet, sizeof(packet));

    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }

    // Reverse the start, end and p:
    packet.start = be64toh(packet.start);
    packet.end = be64toh(packet.end);

    printf("\nStart:   %" PRIu64 "\n", packet.start);
    printf("End:     %" PRIu64 "\n", packet.end);
    printf("P:       %d\n", packet.p);


    printf("Received hash:\n");
    for (i = 0; i < 32; i++){
        printf("%0x", packet.hash[i]);
    }

    // /* SHA 256 ALGO */ 
    uint64_t answer = packet.start;
    uint8_t theHash[32];

    for (answer; answer <= packet.end; answer++){

        bzero(theHash, 32);
        unsigned char *hashedNumber = SHA256((char*) &answer, 8, theHash);


        if (memcmp(theHash, packet.hash, sizeof(theHash)) == 0) {
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
    printf("test2\n");
    answer = htobe64(answer);
    n = write(newSockFileDescripter, &answer, 8);

    if(n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
    
    close(newSockFileDescripter);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}
