/**
* Critical Concurrency Lab
* CS 241 - Fall 2018
*/

#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    /* Your code here */
    queue *result = malloc(sizeof(queue));
    if (result == NULL) {
        return (queue *)NULL;
    }
    result->size = 0;
    result->max_size = max_size;
    result->head = NULL;
    result->tail = NULL;
    pthread_mutex_init(&result->m, NULL);
    pthread_cond_init(&result->cv, NULL);

    return result;
}

void queue_destroy(queue *this) {
    /* Your code here */
    if (this == NULL) {
        return;
    }

    if (this->size != 0) {
        queue_node *temp = this->head;
        while (this->size != 0) {
            this->head = temp->next;
            temp->next = NULL;
            free(temp);
            temp = this->head;
            this->size--;
        }
        temp = NULL;
    }
    this->head = NULL;
    this->tail = NULL;

    pthread_mutex_destroy(&this->m);
    pthread_cond_destroy(&this->cv);

    free(this);
}

void queue_push(queue *this, void *data) {
    /* Your code here */
    pthread_mutex_lock(&this->m);
    if (this->max_size > 0) {
        while (this->size >= this->max_size) {
            pthread_cond_wait(&this->cv, &this->m);
        }
    }

    queue_node *input = malloc(sizeof(queue_node));
    if (input == NULL) {
        return;
    }
    input->data = data;
    input->next = NULL;
    if (this->tail == NULL) {
        this->head = input;
        this->tail = input;
    } else {
        this->tail->next = input;
        this->tail = input;
    }
    this->size++;

    pthread_cond_broadcast(&this->cv);
    pthread_mutex_unlock(&this->m);
}

void *queue_pull(queue *this) {
    /* Your code here */
    pthread_mutex_lock(&this->m);
    while (this->size <= 0) {
        pthread_cond_wait(&this->cv, &this->m);
    }

    queue_node *ret = this->head;
    this->head = this->head->next;
    ret->next = NULL;

    this->size--;
    if (this->size == 0) {
        this->head = NULL;
        this->tail = NULL;
    }
    void *ret_data = ret->data;
    ret->data = NULL;
    free(ret);

    pthread_cond_broadcast(&this->cv);
    pthread_mutex_unlock(&this->m);

    return (void *)ret_data;
}
