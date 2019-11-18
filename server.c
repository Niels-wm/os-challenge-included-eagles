#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "messages.h"
#include "packet.h"
#include "reversehashing.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <pthread.h>
#include <sched.h>
#include <signal.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#define PORT 5003
#define NTHREADS 5

// Scheduler Policy: Round-Robin(RR), FIFO(FIFO).
int policy = SCHED_FIFO;

pthread_mutex_t* lock_flag;
pthread_cond_t* cond;
bool* complete_flag;

pthread_mutex_t* lock_running;

bool wait = false;
void sig_func(int sig) {
    printf("%s\n", "caught signal");
    signal(SIGUSR1, sig_func);
    wait = false;
}


int main(int argc, char *argv[]) {
    int sockFileDescripter, newSockFileDescripter;
    int priority_min, priority_max;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, err;
    int i = 0;
    pthread_t thread_id[NTHREADS];
    pthread_t main_thread = pthread_self();
    int priorities[NTHREADS];
    bool running[NTHREADS];
    printf("\n\n\n\n%s\n", "--------------------------STARTING--------------------");

    sockFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFileDescripter < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    lock_flag = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(lock_flag, NULL);
    lock_running = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(lock_running, NULL);

    
    int j = 0;
    for(j; j < NTHREADS; j++) {
        running[j] = false;
    }
    
    signal(SIGUSR1, sig_func);

    /* Disable safety feature */
    // The operating system sets a timeout on TCP sockets after using them. 
    // It does that to make sure that all information from the old program 
    // is gone before starting a new one
    int option = 1;
    setsockopt(sockFileDescripter, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    /* Initialize socket structure */
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
    listen(sockFileDescripter, 50);
    clientAddrSize = sizeof(clientAddr);

    // Put the accept statement and the following code in an infinite loop
    while (1) {
            struct Packet packet;
            
            pthread_attr_t tattr;
            struct sched_param param;

            /* Accept */
            newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);
            
            if (newSockFileDescripter < 0) {
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

            // Reverse the start and end
            packet.start = be64toh(packet.start);
            packet.end = be64toh(packet.end);

            // Thread priority
            // Max thread priority (RR) is 99. 99/16 ~ 6
            param.sched_priority = 1; //(int) packet.p * 6;

            // Initialize thread attributes.
            err = pthread_attr_init(&tattr);
            if (err != 0){
                perror("Error initializing thread attributes");
            }

            // Set the scheduler policy.
            err = pthread_attr_setschedpolicy(&tattr, policy);
            if (err != 0) {
                perror("Error setting thread scheduler policy");
            }

            // Set the scheduler parameters (priority).
            err = pthread_attr_setschedparam(&tattr, &param);
            if (err != 0) {
                perror("Error setting thread priority");
            }

            //printf("%s\n", "asd");

            // For each client request creates a thread and assign the request to it to process
            struct arg_struct *args = malloc(sizeof(struct arg_struct));
            args -> fileDescripter = newSockFileDescripter;
            args -> packet = packet;
            pthread_mutex_lock(lock_running);
            args -> running = &(running[i%NTHREADS]);
            pthread_mutex_unlock(lock_running);
            args -> lock_running = lock_running;
            args -> main_thread = main_thread;


            

            bool deadThread = false;

            pthread_mutex_lock(lock_running);
            printf("\n%s\n", "Thread status:");
            j = 0;
            for (j;j<NTHREADS;j++) {
                printf("%d: ", j);
                printf(running[j] ? "true\n" : "false\n");
            }
            j = 0;
            for (j;j<NTHREADS;j++) {
                if (!(running[j])) {
                    printf("%s%d, %d\n", "Dead thread, i, j: ", i, j);
                    deadThread = true;
                    running[j] = true;
                    break;
                }
            }

            pthread_mutex_unlock(lock_running);

            if (deadThread) {
                printf("Starting thread_id[%d]\n", ((j)%NTHREADS));
                args -> id = j;

                // Pass attributes when creating thread.
                err = pthread_create(&thread_id[j%NTHREADS], &tattr, reversehashing, args);
                if (err != 0) {
                    perror("ERROR creating thread");
                    exit(1);
                }
            }

            if (i >= NTHREADS-1) {
                wait = true;
                while(wait) {
                    printf("%s\n", "waiting");
                    sleep(1);
                }
               // printf("Waiting for thread_id[%d]\n", ((i+1)%NTHREADS));
                //pthread_join(thread_id[(i+1)%NTHREADS], NULL);
            }
            i++;
    }
}
