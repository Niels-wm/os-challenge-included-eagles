#include <stdlib.h>
#include <sys/types.h>
#include "structs.h"

#include <pthread.h>

struct list_node {
    struct Request request;
    struct list_node* next;
};

struct Request null_request;

struct list_node* head;

pthread_mutex_t lock;

/* Creates the list access mutex. Due to the consumer-producer pattern I'm using,
 * this data structure is being accessed in mutiple threads and I need to be sure 
 * that the list strucutre remains valid. Therefore, list accesses are synchronized
 * with a mutex. 
 *
 * The null_request is used to pass back a 'list is empty' response without having 
 * to allocate memory for the requests. Memory allocation takes time, and is also
 * a single threaded operation, so I do my best to minimize it. The only elements
 * that are allocated are the list nodes themselves and they are confined to this 
 * file, which is all single threaded anyway for consistency reasons. */
void init_list(){
   pthread_mutex_init(&lock, NULL); 
   null_request.reply_socket = -1;
}

/* Pushes an item into the list. Its position is based on its precomputed priority. 
 * To make the list into a FIFO, simply define the FIFO macro. (Note, the FIFO
 * code would be better optimized with a tail pointer instead of a list traversal) */
void push_item(struct Request request){
    struct list_node* to_add = malloc(sizeof(struct list_node));
    to_add->request = request;
    to_add->next = NULL;

    pthread_mutex_lock(&lock);

    if(head == NULL){
        head = to_add;

        pthread_mutex_unlock(&lock);
        return;
    }

    if(to_add->request.prio < head->request.prio){
        to_add->next = head;
        head = to_add;

        pthread_mutex_unlock(&lock);
        return;
    }

    struct list_node* curr = head;
    while(curr->next != NULL){
        #ifndef FIFO
            if(to_add->request.prio < curr->next->request.prio){
                to_add->next = curr->next;
                curr->next = to_add;

                pthread_mutex_unlock(&lock);
                return;
            } 
        #endif
        curr = curr->next;
    }

    curr->next = to_add;

    pthread_mutex_unlock(&lock);
}

/* Pops the head element off of the list and returns it */
struct Request pop_item(){
    pthread_mutex_lock(&lock);

    if(head == NULL){
        pthread_mutex_unlock(&lock);
        return null_request;
    }

    struct Request ret;
    ret = head->request;

    struct list_node* next_head = head->next;
    free(head);
    head = next_head;

    pthread_mutex_unlock(&lock);
    return ret;
}
