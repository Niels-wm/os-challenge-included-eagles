 #include "priorityQueue.h"

// Source: https://www.geeksforgeeks.org/priority-queue-using-linked-list/

struct Node *createNewNode(struct Packet packet, uint8_t p, int fd) {

    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->data = packet;
    temp->priority = p;
    temp->fd = fd;
    temp->next = NULL;

    return temp;
}

struct Node peek(struct Node **head) {
    struct Node *temp = *head;
    return *temp;
}

void pop(struct Node **head) {
    struct Node *temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

void push(struct Node **head, struct Packet d, uint8_t p, int fd) {
    struct Node *start = (*head);

    struct Node *temp = createNewNode(d, p, fd);

    if ((*head)->priority < p) {
        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;

    } else {

        while (start->next != NULL && start->next->priority >= p)
        {
            start = start->next;
        }

        temp->next = start->next;
        start->next = temp;
    }
}

int isEmpty(struct Node **head) {
    return (*head) == NULL;
}