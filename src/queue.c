#include "utility.h"
#include <stdlib.h>
#include <assert.h>
typedef struct Node {
    void* data;
    struct Node* next;
}Node;

typedef struct Queue {
    Node* head;
    Node* tail;
    void (*free_data)(void* data);
} Queue;

Queue* queue_create(void(*free_data)(void* data)) {
    Queue* queue = malloc(sizeof(Queue));
    assert(queue);
    queue->head = NULL;
    queue->tail = NULL;
    queue->free_data= free_data;
    return queue;
}

void queue_destroy(Queue* que) {
    if(que) {
        while(que->head) {
            Node* temp = que->head;
            que->head = que->head->next;
            que->free_data(temp->data);
            free(temp);
        }
        free(que);
    }
}

void queue_push(Queue* restrict que, void* data) {
    Node* node = malloc(sizeof(Node));
    assert(node);
    node->data = data;
    node->next = NULL;
    if(que) {
        if(que->head && que->tail) {
            que->tail->next = node;
            que->tail = que->tail->next;
        }
        else {
            /* first element */
            que->head = node;
            que->tail = node;
        }
    }
}

void* queue_pop(Queue* restrict que) {
    if(que) {
        if(que->head) {
            /* check that was only one element */
            void* ret;
            ret = que->head->data;
            if(que->head == que->tail) {
                free(que->head);
                que->head = NULL;
                que->tail = NULL;
            }
            else {
                Node* temp;
                temp = que->head;
                que->head = que->head->next;
                free(temp);
            }
            return ret;
        }
    }
    return NULL;
}

bool queue_empty(Queue* restrict que) {
    if(que->head == NULL && que->tail == NULL)
        return true;
    return false;
}

