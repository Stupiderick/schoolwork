/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "vector.h"
#include<stdio.h>
int main() {
    // Write your test cases here
    vector *erick = vector_create(int_copy_constructor, int_destructor, int_default_constructor);

    int zero = 0;
    int one = 1;
    int two = 2;
    int three = 3;
    int four = 4;


    vector_push_back(erick, &zero);
    printf("2\n");
    vector_push_back(erick, &one);
    printf("3\n");
    vector_push_back(erick, &two);
    vector_push_back(erick, &three);

    vector_resize(erick, 20);
    printf("capacity: %zu\n", vector_capacity(erick));
    printf("size: %zu\n", vector_size(erick));
    

    vector_resize(erick, 4);
    printf("capacity: %zu\n", vector_capacity(erick));
    printf("size: %zu\n", vector_size(erick));


    vector_pop_back(erick);
    printf("capacity: %zu\n", vector_capacity(erick));
    printf("size: %zu\n", vector_size(erick));

    printf("get 2: %d\n", *(int *)vector_get(erick, 1));

    vector_set(erick, 1, &three);
    printf("get 0-3: %d, %d, %d\n", *(int *)vector_get(erick, 0), *(int *)vector_get(erick, 1), *(int *)vector_get(erick, 2));
    

    vector_insert(erick, 2, &four);
    printf("get 0-3: %d, %d, %d\n", *(int *)vector_get(erick, 0), *(int *)vector_get(erick, 1), *(int *)vector_get(erick, 2));
    
    printf("capacity: %zu\n", vector_capacity(erick));
    printf("size: %zu\n", vector_size(erick));
    
    vector_erase(erick, 1);
    printf("get 0-3: %d, %d, %d\n", *(int *)vector_get(erick, 0), *(int *)vector_get(erick, 1), *(int *)vector_get(erick, 2));

    vector_resize(erick, 0);


    
    printf("destroy\n");
    vector_destroy(erick);

    return 0;
}
