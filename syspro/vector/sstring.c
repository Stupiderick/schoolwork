/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    vector *str;   
};

sstring *cstr_to_sstring(char *input) {
    // your code goes here
    assert(input);

    sstring *ptr = malloc(sizeof(sstring));
    ptr->str = vector_create(char_copy_constructor, char_destructor, char_default_constructor);

    for (int i = 0; i < (int)strlen(input); i++) {
        vector_push_back(ptr->str, input + i);
    } 
    return ptr;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
    size_t len = vector_size(input->str);
    char *output = (char *)malloc((int)len + 1);
    
    for (size_t i = 0; i < len; i++) {
        output[i] = *(char *)vector_get(input->str, i);
    }
    output[len] = '\0';
    
    return output;
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
    assert(this);
    assert(addition);

    for (size_t i = 0; i < vector_size(addition->str); i++) {
        vector_push_back(this->str, (char *)vector_get(addition->str, i));
    }
    
    return (int)vector_size(this->str);
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
    assert(this);
    vector *vtr = vector_create(string_copy_constructor, string_destructor, string_default_constructor);
    int flag = 0;
    int posi = 0;
    char *temp;
    for (int i = 0; i < (int)vector_size(this->str); i++) {
        char t = *(char *)vector_get(this->str, i);
        // if there needs a new string..
        if (flag == 0) {
            temp = malloc(posi + 1);
            flag = 1;
        }
        if (t == delimiter) {
            temp[posi] = '\0';
            flag = 0;
            vector_push_back(vtr, temp);
            free(temp);
            temp = NULL;
            posi = 0;
        } else {
            temp[posi] = t;
            temp = realloc(temp, posi + 2);
            posi++;
        }
    }
    // when the delimiter is the last char.
    if (flag == 0) {
        char *temp1 = malloc(1);
        temp1[0] = '\0';
        vector_push_back(vtr, temp1);
        free(temp1);
        temp1 = NULL;
    } else {
        temp[posi] = '\0';
        vector_push_back(vtr, temp);
    }
    
    free(temp);
    temp = NULL;

    return vtr;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    // your code goes here
    sstring *tar = cstr_to_sstring(target);
    sstring *sub = cstr_to_sstring(substitution);

    size_t prevSize = vector_size(this->str);
//    size_t posi = 0;
    int first = -1;
    int last = -1;
    for (size_t i = offset; i < prevSize; i++) {
        if (*(char *)vector_get(this->str, i) == target[0]) {
            size_t flag = 0;
            first = i;
            for (size_t j = 1; j < vector_size(tar->str); j++) {
                if (i + j == prevSize && j != strlen(target)) {
                    flag = 1;
                    first = -1; 
                    i = i + j + 1;
                    break;
                } else if (i + j == prevSize && j == strlen(target)) {
                    break;
                }
                if (*(char *)vector_get(this->str, i + j) != target[j]) {
                    flag = 1;
                    first = -1;
                    i = i + j + 1;
                    break;
                }
                
            }
            if (flag == 0) {
                last = i + vector_size(tar->str);
                break;
            }
        }
    }
    if (first == -1 && last == -1) {
        sstring_destroy(tar);
        sstring_destroy(sub);
        return -1;
    } else {
        for (int k = first; k < last; k++) {
            vector_erase(this->str, first);
        }

        for (int k = 0; k < (int)vector_size(sub->str); k++) {
            vector_insert(this->str, (size_t)(first + k), (substitution + k));
        }
    }   
    sstring_destroy(tar);
    sstring_destroy(sub);

    return 0;
}

char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
    assert(this);
    //assert(this->str != NULL);
    if (end == (int)vector_size(this->str) + 1) {
        end = vector_size(this->str);
    }

    char *output = malloc((end - start) * sizeof(char) + 2);
    for (int i = start - 1; i < end; i++) {
        output[i - start + 1] = *(char *)vector_get(this->str, i);
    }
    output[end - start + 1] = '\0';
    return output;
}

void sstring_destroy(sstring *this) {
    // your code goes here
    vector_destroy(this->str);
    free(this);
    this = NULL;
}
