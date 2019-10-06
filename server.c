#include <stdio.h>
#include <stdlib.h>
#include "messages.h"
#include <sys/mman.h>
#include<signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <inttypes.h>

#include "splayTree.c"

#define PORT 5003

//uint64_t reversehashing (struct Packet packet1);

struct Packet {
   uint8_t hash[32];
   uint64_t start;
   uint64_t end;
   uint8_t p;
};

uint64_t reversehashing (struct Packet packet1) {
  int i;

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
    //uint64_t answer2 = 5;

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
      return answer;


}

typedef struct Hej {
   int fds;
   uint64_t pakke;
} Hej;

int newSockFileDescripter;
int fd[2];
Hej *modtaget;



void handler(int sig) {
  printf("SIGNAL: %d\n", sig);
  int n;
  uint64_t answer;
  printf("\nSignal invoked\n");
  read(fd[0], modtaget, sizeof(Hej));
  printf("%p\n", modtaget);

  printf("Sending     %" PRIu64 "\n", modtaget->pakke);
  printf("\nnow receieved\n");
  modtaget->pakke = htobe64(modtaget->pakke);
  printf("newfds: %d\n", modtaget->fds);
  n = write(modtaget->fds, &(modtaget->pakke) ,8);

  if(n < 0) {
    printf("ERROR WRITING TO SOCKET\n" );
      perror("ERROR writing to socket");
      exit(1);
  }
}

int main(int argc, char *argv[]) {
    int sockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, i, pid;

    modtaget = mmap(NULL, sizeof(Hej), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sockFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFileDescripter < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    signal(SIGCHLD, handler);

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

    struct Packet packet1;
    if (pipe(fd) == -1) {
      printf("pipe fail\n");
    }

    // Put the accept statement and the following code in an infinite loop
    while (1) {

            /* Accept */
            newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);
            if (newSockFileDescripter < 0){
                perror("ERROR on accept");
                exit(1);
            }

            printf("newfds: %d\n", newSockFileDescripter);
            printf("read: %d\n", fd[0]);
            printf("write: %d\n", fd[1]);


            /* Recive */
            bzero((char *)&packet1, sizeof(packet1));
            n = read(newSockFileDescripter, &packet1, sizeof(packet1));

            if (n < 0) {
                perror("ERROR reading from socket");
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

                uint64_t answer = reversehashing(packet1);
                modtaget->pakke = answer;
                modtaget->fds = newSockFileDescripter;
                printf("\nHave answer\n");
                printf("\nNow writing:     %" PRIu64 "\n", modtaget->pakke);
                write(fd[1], modtaget, sizeof(modtaget));
                printf("\nSent\n");
                exit(0);
            } else {
                if(n < 0) {
                    perror("ERROR writing to socket");
                    exit(1);
                }
            }

    }
}
