/**
* Mini Valgrind Lab
* CS 241 - Fall 2018
*/

#include "mini_valgrind.h"
#include <stdio.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

size_t total_memory_requested = 0;
size_t total_memory_freed = 0;
size_t invalid_addresses = 0;
meta_data* head = NULL;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
    if (request_size <= 0) {
        return NULL;
    }

    meta_data *ret = (meta_data *)malloc(sizeof(meta_data) + request_size);
    if (ret == NULL) return NULL;

    // set up meta_data...
    ret->request_size = request_size;
    ret->filename = filename;
    ret->instruction = instruction;
    ret->next = NULL;

    // set up extern head...
    // if there is no meta_data struct, let head point to ret...
    if (head == NULL) {
         head = ret;
    // or traverse to the end of the list...
    } else {
        meta_data *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = ret;
        temp = NULL;
    }
    
    // set up extern total_request_size...
    total_memory_requested += request_size;

    // go to real memory space...
    ret++;

    return (void *)ret;
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    // your code here
    if (num_elements == 0 || element_size == 0) {
        return NULL;
    }

    void *ret = mini_malloc(num_elements * element_size, filename, instruction);
    if (ret == NULL) {
        return NULL;
    } else {
        return memset(ret, 0, num_elements * element_size);
    }
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
    if (payload == NULL && request_size != 0) {
        return mini_malloc(request_size, filename, instruction);
    } else if (payload != NULL && request_size == 0) {
        mini_free(payload);
        return NULL;
    } else if (payload == NULL && request_size == 0) {
        return NULL;
    }

    if (head == NULL) {
        invalid_addresses++;
        return (void *)NULL;
    }


    meta_data *temp = (meta_data *)payload;
    temp--;
    if (head == temp) {
        temp = (meta_data *)realloc(temp, sizeof(meta_data) + request_size);
        head = temp;
        temp->filename = filename;
        temp->instruction = instruction;

        if (request_size > temp->request_size) {
            total_memory_requested += (request_size - temp->request_size);
        } else if (request_size < temp->request_size) {
            total_memory_freed += (temp->request_size - request_size);
        }
        temp->request_size = request_size;
        temp++;
        payload = NULL;

        return (void *)temp;
    }

    meta_data *trav = head;
    bool flag = false;
    while (trav->next != NULL) {
        if (trav->next == temp) {
            flag = true;
            break;
        } else {
            trav = trav->next;
        }
    }

    if (!flag) {
        invalid_addresses++;
        return (void *)NULL;
    }

    temp = (meta_data *)realloc(temp, sizeof(meta_data) + request_size);
    trav->next = temp;
    
    temp->filename = filename;
    temp->instruction = instruction;

    if (request_size > temp->request_size) {
        total_memory_requested += (request_size - temp->request_size);
    } else if (request_size < temp->request_size) {
        total_memory_freed += (temp->request_size - request_size);
    }
    trav = NULL;
    temp->request_size = request_size;
    temp++;
    payload = NULL;
    return (void *)temp;
}

void mini_free(void *payload) {
    // your code here
    if (payload == NULL) {
        return;
    }

    if (head == NULL) {
        invalid_addresses++;
        return;
    }

    meta_data *temp = (meta_data *)payload;
    temp--;

    if (head == temp) {
        head = temp->next;
        temp->next = NULL;
        temp->filename = NULL;
        temp->instruction = NULL;
        
        total_memory_freed += temp->request_size;

        free(temp);
        payload = NULL;
        return;
    }

    meta_data *trav = head;
    bool flag = false;
    while (trav->next != NULL) {
        if (trav->next == temp) {
            flag = true;
            break;
        } else {
            trav = trav->next;
        }
    }

    if (!flag) {
        invalid_addresses++;
        return;
    }

    trav->next = temp->next;
    temp->next = NULL;
    temp->filename = NULL;
    temp->instruction = NULL;
    total_memory_freed += temp->request_size;
    free(temp);
    payload = NULL;
    return;
}

