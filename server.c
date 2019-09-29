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

void reversehashing (int sock);

struct Packet {
   uint8_t hash[32];
   uint64_t start;
   uint64_t end;
   uint8_t p;
};

int main(int argc, char *argv[]) {
    int sockFileDescripter, newSockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, i, pid;

    sockFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFileDescripter < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Disable safety feature */
    // The operating system sets a timeout on TCP sockets after using them. 
    // It does that to make sure that all information from the old program 
    // is gone before starting a new one
    int option = 1;
    setsockopt(sockFileDescripter, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));


    /* Initialize socket structure */
    // bzero() is used to set all the socket structures with null values. It does the same thing as the following:
    // memset(&serverAddr, '\0', sizeof(serverAddr));
    bzero((char *)&serverAddr, sizeof(serverAddr));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;


    /* Bind */
    if (bind(sockFileDescripter, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        perror("ERROR on binding");
        exit(1);
    }


    /* Listen */
    listen(sockFileDescripter, 5);
    clientAddrSize = sizeof(clientAddr);



    // Put the accept statement and the following code in an infinite loop
    while (1) {
            
            /* Accept */
            newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);
            
            if (newSockFileDescripter < 0){
                perror("ERROR on accept");
                exit(1);
            }

            /* Create child process */
            pid = fork();

            if (pid < 0) {
                perror("ERROR on fork");
                exit(1);
            }
            
            if (pid == 0) {
                /* This is the client process */
                close(sockFileDescripter);
                reversehashing(newSockFileDescripter);
                exit(0);
            } else {
                close(newSockFileDescripter);
            }

    }
}



void reversehashing (int sock) {
    struct Packet packet1;
    int n, i;



    /* Recive */
    bzero((char *)&packet1, sizeof(packet1));
    n = read(sock, &packet1, sizeof(packet1));

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
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
    uint64_t answer = packet1.start;
    uint8_t theHash[32];

    for (answer; answer <= packet1.end; answer++){

        bzero(theHash, 32);
        unsigned char *hashedNumber = SHA256((char*) &answer, 8, theHash);


        if (memcmp(theHash, packet1.hash, sizeof(theHash)) == 0) {
            printf("Found a match, with:  %" PRIu64, answer);
            break;
        }
    }

    
    printf("\nHere are the calculated hash:\n");
    for (i = 0; i < 32; i++){
        printf("%0x", theHash[i]);
    }
    printf("\n");


    /* Send */
    answer = htobe64(answer);
    n = write(sock, &answer ,8);

    if(n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}
