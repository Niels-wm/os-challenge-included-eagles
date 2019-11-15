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

void init_list(){
   pthread_mutex_init(&lock, NULL); 
   null_request.reply_socket = -1;
}

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
        if(to_add->request.prio < curr->next->request.prio){
            to_add->next = curr->next;
            curr->next = to_add;

            pthread_mutex_unlock(&lock);
            return;
        } 
        curr = curr->next;
    }

    curr->next = to_add;

    pthread_mutex_unlock(&lock);
}

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
