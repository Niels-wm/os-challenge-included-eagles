#include <stdio.h>
#include <stdlib.h>
#include "messages.h"
#include <sys/mman.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <inttypes.h>

#define PORT 5003

typedef struct Node {
    struct Node *parent;
    struct Node *right;
    struct Node *left;
    uint8_t hash;
    uint64_t value;
} Node;

Node *root = NULL;

void splay(Node *node) {
    Node *p = node->parent;
    if (!p) {
        return;
    }

    Node *pp = node->parent->parent;
    Node *left = node->left;
    Node *right = node->right;

    // Zig operation
    if (!pp) {
        if (p->left == node) {
            /*if (pp != NULL) {
                if (pp->left == pp) {
                    pp->left = node;
                } else {
                    pp->right = node;
                }
            }*/
            if (node->right != NULL) {
                p->left = node->right;
                p->left->parent = p;
            } else {
                p->left = NULL;
            }

            node->right = p;
            node->parent = pp;
            node->right->parent = node;

        } else {
            /*if (pp != NULL) {
                if (pp->right == pp) {
                    pp->right = node;
                } else {
                    pp->left = node;
                }
            }*/
            if (node->left != NULL) {
                p->right = node->left;
                p->right->parent = p;
            } else {
                p->right = NULL;
            }

            node->left = p;
            node->parent = pp;
            node->left->parent = node;
        }
    } else if (p->left == node) {
        // Zig zig right rotation
        if (pp->left == p) {
            Node *ppp = pp->parent;
            node->parent = ppp;
            if (ppp != NULL) {
                if (ppp->left == pp) {
                    ppp->left = node;
                } else {
                    ppp->right = node;
                }
            }
            if (p->right != NULL) {
                pp->left = p->right;
                pp->left->parent = pp;
            } else {
                pp->left = NULL;
                p->parent = NULL;
            }

            if (node->right != NULL) {
                p->left = node->right;
                p->left->parent = p;
            } else {
                p->left = NULL;
            }
            p->right = pp; // 2
            pp->parent = p;
            node->right = p;
            p->parent = node;
        } else { // Zig zag Left-right
            Node *ppp = pp->parent;
            node->parent = ppp;

            if (ppp != NULL) {
                if (ppp->left == pp) {
                    ppp->left = node;
                } else {
                    ppp->right = node;
                }
                pp->parent = node;
            }

            if (node->left != NULL) {
                pp->right = node->left;
                pp->right->parent = p;
            } else {
                pp->left = NULL;
            }

            if (node->right != NULL) {
                p->left = node->right;
                p->left->parent = p;
            } else {
                p->left = NULL;
            }

            node->right = p;

            node->left = pp;
            node->right->parent = node;
            node->left->parent = node;
            node->parent = ppp;
        }
        splay(node);
    } else if (p->right == node) {
        // Zig zig
        if (pp->right == p) {
            if (pp->parent != NULL) {
                if (pp->parent->left == pp) {
                    pp->parent->left = node;
                } else {
                    pp->parent->right = node;
                }
            }
            if (p->left != NULL) {
                pp->right = p->left;
                pp->right->parent = pp;
            } else {
                pp->right = NULL; // 1
            }

            p->left = pp; // 2

            if (node->left != NULL) {
                p->right = node->left;
                node->left->parent = p;
            } else {
                p->right = NULL;
            }

            node->left = p;

            p->parent = node;
            node->parent = pp->parent;
            pp->parent = p;
        } else { // Zig zag Right-left
            Node *ppp = pp->parent;
            node->parent = ppp;

            if (ppp != NULL) {
                if (ppp->right == pp) {
                    ppp->right = node;
                } else {
                    ppp->left = node;
                }
                pp->parent = node;
            }

            if (node->right != NULL) {
                pp->left = node->right;
                pp->left->parent = p;
            } else {
                pp->left = NULL;
            }

            if (node->left != NULL) {
                p->right = node->left;
                p->right->parent = p;
            } else {
                p->right = NULL;
            }

            node->left = p;

            node->right = pp;
            node->right->parent = node;
            node->left->parent = node;
        }
        splay(node);
    }
//    root = node;
}

void insert(uint8_t *newHash, uint64_t value, struct Node *node) {

    if (!node) {
        if (!root) {
            root = (Node *) mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
            root->parent = NULL;
            root->right = NULL;
            root->left = NULL;

/*            for (int i = 0; i < 32; ++i) {
                root->hash[i] =  newHash[i];
            }*/
            root->hash = *newHash;
            root->value = value;
            return;
        }

        insert(newHash, value, root);
        return;
    }
    if (node->hash > *newHash) {
        if (!node->left) {
            node->left = (Node *) mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
            node->left->parent = node;
            /*for (int i = 0; i < 32; ++i) {
                node->left->hash[i] = newHash[i];
            }*/
            node->left->hash = *newHash;
            node->left->value = value;
            root = node->left;
            splay(node->left);
        } else {
            insert(newHash, value, node->left);
        }
    } else if (node->hash < *newHash) {
        if (!node->right) {
            node->right = (Node *) mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
            node->right->parent = node;
            node->right->hash = *newHash;
            node->right->value = value;
            root = node->right;
            splay(node->right);
        } else {
            insert(newHash, value, node->right);
        }
    } else {
        root = node;
        splay(node);
    }
}

uint64_t find(uint8_t *n, struct Node *node) {
  printf("Looking for : " );
  for (size_t i = 0; i < 32; i++) {
    printf("%0x", n[i]);
  }
  printf("\n");

    if (!root)
        return 0;

    if (!node)
        return find(n, root);

        printf("In node : " );
        for (size_t i = 0; i < 32; i++) {
          printf("%0x", node->);
        }

    if (node->hash < *n) {
        if (!node->right) {
            splay(node);
        } else {
            return find(n, node->right);
        }
    } else if (node->hash > *n) {
        if (!node->left) {
            splay(node);
        } else {
            return find(n, node->left);
        }
    } else {
        splay(node);
        return node->value;
    }

    return 0;
}

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

    printf("\nHere are the received hash:\n");
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

typedef struct SplayInfo {
   uint8_t *hash;
   uint64_t value;
} SplayInfo;

int newSockFileDescripter;
int fd[2];
SplayInfo *info;

void handler(int sig) {
  insert(info->hash, info->value, NULL);
  printf("Inserted %" PRIu64 "\n", root->value);

  //n = write(info->fds, &(info->value) ,8);

  // if(n < 0) {
  //   perror("ERROR writing to socket");
  //   exit(1);
  // }
}

int main(int argc, char *argv[]) {
    int sockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, i, pid;

    // root = (Node*) mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    info = mmap(NULL, sizeof(SplayInfo), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

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

            /* Recive */
            bzero((char *)&packet1, sizeof(packet1));
            n = read(newSockFileDescripter, &packet1, sizeof(packet1));

            if (n < 0) {
                perror("ERROR reading from socket");
                exit(1);
            }
            for (size_t i = 0; i < 32; i++) {
              printf("%0x",packet1.hash[i]);
            }
            printf("\n");

            uint64_t value = find(packet1.hash, NULL);

            if (value != 0) {
              printf("\nFound:     %" PRIu64 "\n", value);
              n = write(newSockFileDescripter, &value ,8);

              if(n < 0) {
                perror("ERROR writing to socket");
                exit(1);
              }
            } else {
              printf("\nDIDNT FIND ANYTHING\n");
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
                  printf("\nNow writing:     %" PRIu64 "\n", answer);
                  printf("ANSWER: %" PRIu64 "\n", answer);
                  answer = htobe64(answer);
                  // int r = rand() % 20;
                  // sleep(r);
                  n = write(newSockFileDescripter, &answer ,8);

                  if(n < 0) {
                    perror("ERROR writing to socket");
                    exit(1);
                  }
                  printf("ANSWER2: %" PRIu64 "\n", answer);
                  info->value = answer;
                  info->hash = packet1.hash;
                  exit(0);
              } else {
                  if(n < 0) {
                      perror("ERROR writing to socket");
                      exit(1);
                  }
              }
            }



    }
}
