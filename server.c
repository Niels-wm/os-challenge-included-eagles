#include <stdio.h>
#include <stdlib.h>
#include "messages.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <inttypes.h>

#define PORT 5003

struct Packet {
   uint8_t hash[32];
   uint64_t start;
   uint64_t end;
   uint8_t p;
};

int main(int argc, char *argv[]) {
    int sockFileDescripter, newSockFileDescripter;
    struct Packet packet1;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, i;


    sockFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFileDescripter < 0) {
        error("ERROR opening socket");
    }

    /* Initialize socket structure */
    // bzero() is used to set all the socket structures with null values. It does the same thing as the following:
    // memset(&serverAddr, '\0', sizeof(serverAddr));
    bzero((char *)&serverAddr, sizeof(serverAddr));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;


    /* Bind */
    if (bind(sockFileDescripter, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        error("ERROR on binding");
    }


    /* Listen */
    listen(sockFileDescripter, 5);
    clientAddrSize = sizeof(clientAddr);


    /* Accept */
    newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (newSockFileDescripter < 0){
        error("ERROR on accept");
    }


    /* Recive */
    n = read(newSockFileDescripter, &packet1, 49);

    if (n < 0) {
        error("ERROR reading from socket");
    }

    // Reverse the start, end and p:
    packet1.start = be64toh(packet1.start);
    packet1.end = be64toh(packet1.end);

    
    printf("Here are the received hash:\n");
    for (i = 0; i < 32; i++){
        printf("%0x", packet1.hash[i]);
    }

    printf("\nHere are the start:   %" PRIu64 "\n", packet1.start);
    printf("Here are the end:     %" PRIu64 "\n", packet1.end);
    printf("Here are the p:       %d\n", packet1.p);



    /* SHA 256 ALGO */ 
    printf("\nStarting the Reverse Hashing (Brute Force) Algorithm:\n");
    uint64_t j = packet1.start;
    uint8_t TheHash[32];

    for (j; j <= packet1.end; j++){

        uint64_t numberToHash = j;
        bzero(TheHash, 32);
        unsigned char *hashedNumber = SHA256((char*) &numberToHash, 8, TheHash);


        if (memcmp(TheHash, packet1.hash, sizeof(TheHash)) == 0) {
            printf("Found a match, with:  %" PRIu64, j);
            break;
        }
    }

    
    printf("\nHere are the calculated hash:\n");
    for (i = 0; i < 32; i++){
        printf("%0x", TheHash[i]);
    }
    printf("\n");


    /* Send */
    uint64_t answer = htobe64(j);
    n = write(newSockFileDescripter, &answer ,8);

    if(n < 0) {
        error("ERROR writing to socket");
    }

    
    return 0;
}