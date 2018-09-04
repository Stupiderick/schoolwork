#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct Person person_t;
struct Person {
    
    char * name;
    int age;
    person_t **s;
};

person_t * create(char * aName, int anAge) {
    person_t * res = (person_t*) malloc(sizeof(person_t));
    res->name = strdup(aName);
    res->age = anAge;
    res->s = malloc(sizeof(person_t*) * 10);
    
    return res;
}


void destroy(person_t * p) {
    free(p->name);
    free(p->s);
    memset(p, 0, sizeof(person_t));
    free(p);
}


int main() {
//    person_t * as = (person_t*) malloc(sizeof(person_t));
//    person_t * sm = (person_t*) malloc(sizeof(person_t));
//    as->name = "Agent Smith";
//    sm->name = "Sonny Moore";
//    as->age = 128;
//    sm->age = 256;
//    as->s = &sm;
//    sm->s = &as;
//
//    free(as);
//    free(sm);
    person_t * p = create("Erick", 23);
    destroy(p);
    
    return 0;
}

