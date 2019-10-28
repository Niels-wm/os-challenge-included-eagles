#include <stdio.h>
#include <stdlib.h>
#include "messages.h"
#include "packet.h"
#include "reversehashing.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include "node.h"
#include "priorityQueue.h"

#define PORT 5003

float getScoreAlgo(struct Packet packet);

int main(int argc, char *argv[]) {
    int sockFileDescripter, newSockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, i, pid;
    float pScore = 0.0;
    struct Node *pq;


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
    listen(sockFileDescripter, 10);
    clientAddrSize = sizeof(clientAddr);


    // Put the accept statement and the following code in an infinite loop
    while (1) {

            struct Packet packet;
            int n;

            /* Accept */
            newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);

            if (newSockFileDescripter < 0){
                perror("ERROR on accept");
                exit(1);
            }

            /* Receive */
            bzero((char *)&packet, sizeof(packet));
            n = read(newSockFileDescripter, &packet, sizeof(packet));

            if (n < 0) {
                perror("ERROR reading from socket");
                exit(1);
            }

            // Reverse the start, end and p:
            packet.start = be64toh(packet.start);
            packet.end = be64toh(packet.end);

            // printf("\nStart:   %" PRIu64 "\n", packet.start);
            // printf("End:     %" PRIu64 "\n", packet.end);
            // printf("P:       %d\n", packet.p);

            // -- CHECK IF RECEIVED HASH IS A KNOWN HASH (IN HASHTABLE) AND SEND ANSWER TO CLIENT IF IT IS:



            // -- ALGO
            
            // pScore = getScoreAlgo(packet);
            // printf("Score:       %f\n", pScore);

            // -- IMPLEMENT THE SCHEDULER HERE:

            if (isEmpty(&pq)){
                pq = createNewNode(packet, packet.p, newSockFileDescripter);
            } else {
                push(&pq, packet, packet.p, newSockFileDescripter);
            }

            // -- POP THE MOST IMPORTANT PACKET AND NEWSOCKFILEDESCRIPTER HERE:

            struct Node node1 = peek(&pq);
            pop(&pq);

            printf("\nfromQ Start:   %" PRIu64 "\n", node1.data.start);
            printf("fromQ End:     %" PRIu64 "\n", node1.data.end);
            printf("fromQ P:       %d\n", node1.data.p);
            printf("fromQ fd:      %d\n", node1.fd);

            /* Create child process */
            pid = fork();

            if (pid < 0) {
                perror("ERROR on fork");
                exit(1);
            }
            
            if (pid == 0) {
                /* This is the client process */
                close(sockFileDescripter);
                reversehashing(node1.data, node1.fd);
                exit(0);
            } else {
                /* This is the parent process */
                close(newSockFileDescripter);
            }

    }
}

float getScoreAlgo(struct Packet packet) {

    float deltaT = packet.end - packet.start;
    float score = ((float)packet.p/deltaT) * 10000000000;

    return score;
}
