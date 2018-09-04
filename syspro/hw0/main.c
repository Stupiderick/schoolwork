#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int main() {
    char file[] = "fil.txt";
    FILE *myfile = fopen(file, "r");
    char *buffer = NULL;
    size_t capacity = 0;

    while (getline(&buffer, &capacity, myfile) != -1) {
        printf("%s", buffer);
    }

    free(buffer);
    return 0;
}
