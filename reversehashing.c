#include "packet.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>

void *reversehashing(void *arg) {
    
    struct Packet packet;
    struct arg_struct *args = (struct arg_struct *) arg;

    int newSockFileDescripter = args -> fileDescripter;
    packet = args -> packet;
    int i, n;
    //pthread_mutex_t lock_running = args->lock_running;

    /*
    pthread_mutex_lock(args->lock_running);
    //printf(args->running ? "r: true\n" : "r: false\n");
    *(args->running) = 1;
    pthread_mutex_unlock(args->lock_running);
    */

    //printf(args->running ? "true\n" : "false\n");


    /* SHA 256 ALGO */ 
    uint64_t answer = packet.start;
    uint8_t theHash[32];

    for (answer; answer <= packet.end; answer++){

        bzero(theHash, 32);
        unsigned char *hashedNumber = SHA256((char*) &answer, 8, theHash);


        if (memcmp(theHash, packet.hash, sizeof(theHash)) == 0) {
            // printf("\nFound a match, with:  %" PRIu64, answer);
            break;
        }
    }

    /* Send */
    answer = htobe64(answer);
    n = write(newSockFileDescripter, &answer, 8);

    if(n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
    //args->running = false;

    pthread_mutex_lock(args->lock_running);
    //printf(args->running ? "r: true\n" : "r: false\n");
    *(args->running) = 0;
    pthread_mutex_unlock(args->lock_running);

    printf("%s%d\n", "Exiting thread: ", args->id);
    pthread_kill(args->main_thread, SIGUSR1);

    close(newSockFileDescripter);
    
    free(args);
    pthread_exit(NULL);
    return NULL;
}
