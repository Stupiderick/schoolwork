/**
* Extreme Edge Cases Lab
* CS 241 - Fall 2018
*/

#include "camelCaser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>


char **camel_caser(const char *input_str) {
    // case when the input string is NULL
    if (input_str == NULL) {
        return NULL;
    }
    char **res = (char **)malloc(sizeof(char *));
    int lenInput = strlen(input_str);
    // 1 means a new sentence is needed; while 0 means a sentence is writing.
    int senHead = 1;
    // 0 means lowercase needed; while 1 means capitalized letters needed.
    int capNeeded = 0;
    // for individual sentences
    char *sentence = malloc(1);
    // counting how many words in the sentence so far, not including '\0' byte.
    int letterCount = 0;
    // counting how many sentences so far, not including the NULL in the end.
    int senCount = 1;
    int wordCount = 1;

    for (int i = 0; i < lenInput; i++) {
        // check if a period is read.
        if (ispunct(input_str[i])) {
            sentence[letterCount] = '\0';
            res[senCount - 1] = sentence;
            sentence = NULL;
            sentence = malloc(1);
            senHead = 1;
            letterCount = 0;
            wordCount = 1;
            res = (char **)realloc(res, sizeof(char *) * (senCount + 1));
            senCount++;
            // check if a white space is read.
        } else if (isspace(input_str[i])) {
            capNeeded = 1;
        } else if (isalpha(input_str[i])) {
            if (senHead == 1) {
                // senHead to 0, sentence is writing...
                capNeeded = 0;
                senHead = 0;
                sentence = realloc(sentence, letterCount + 2);
                sentence[letterCount] = tolower(input_str[i]);
            } else {
                sentence = realloc(sentence, letterCount + 2);
                if (capNeeded && wordCount != 1) {
                    sentence[letterCount] = toupper(input_str[i]);
                    capNeeded = 0;
                } else {
                    sentence[letterCount] = tolower(input_str[i]);
                }
            }

            letterCount++;
            if (input_str[i + 1] != '\0' && (ispunct(input_str[i + 1]) || isspace(input_str[i + 1]))) {
                wordCount++;
            }
        } else {
            sentence = realloc(sentence, letterCount + 2);
            sentence[letterCount] = input_str[i];
            letterCount++;
            senHead = 0;
            if (input_str[i + 1] != '\0' && (ispunct(input_str[i + 1]) || isspace(input_str[i + 1]))) {
                wordCount++;
            }
        }
    }
    res[senCount - 1] = NULL;
    //sentence = NULL;
    free(sentence);
    sentence = NULL;

    return res;
}

void destroy(char **result) {
    char **temp = result;
    while (*temp != NULL) {
        free(*temp);
        *temp = NULL;
        temp++;
    }

    free(result);
    result = NULL;

    return;
}
