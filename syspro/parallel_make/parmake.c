/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "queue.h"

void cleanTargetsStates(graph *theGraph, vector *tgts);
void print_all_targets(graph *theGraph, vector *tgts);
int checkCycle(graph *theGraph, vector *tgts, char *goal);
int checkDFS(graph *theGraph, void *v);
int graphRun(graph *theGraph, vector *tgts, char *goal);
int DFS(graph *theGraph, void *v);

// set *cycle_rule;

int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    graph *mygraph = parser_parse_makefile(makefile, targets);
    vector *vtargets = graph_vertices(mygraph);
    // cycle_rule = char_set_create();
    int k = 0;

    // fprintf(stderr, "yes: %d\n", targets[0] == NULL);
    // mygraph = parser_parse_makefile(makefile, targets);
    // vtargets = graph_vertices(mygraph);

    if (targets[0] == NULL) {
        // mygraph = parser_parse_makefile(makefile, targets);
        // vtargets = graph_vertices(mygraph);
        if (checkCycle(mygraph, vtargets, NULL) == -1) {
            graph_destroy(mygraph);
            vector_destroy(vtargets);
            return -1;
        }

        cleanTargetsStates(mygraph, vtargets);
        graphRun(mygraph, vtargets, NULL);

        graph_destroy(mygraph);
        vector_destroy(vtargets);
        // queue_destroy(myqueue);

        return 0;
    }

    while (1) {
        // fprintf(stderr, "helo\n");
        // fprintf(stderr, "%s\n", targets[k]);
        if (targets[k] == NULL) {
            // fprintf(stderr, "yess\n");
            break;
        }

        // mygraph = parser_parse_makefile(makefile, targets);
        // vtargets = graph_vertices(mygraph);
        // fprintf(stderr, "k: %s\n", targets[k]);

        if (checkCycle(mygraph, vtargets, targets[k]) == -1) {
            // graph_destroy(mygraph);
            // vector_destroy(vtargets);
            cleanTargetsStates(mygraph, vtargets);
            k++;
            continue;
        }

        // mygraph = parser_parse_makefile(makefile, targets);
        // vtargets = graph_vertices(mygraph);
        cleanTargetsStates(mygraph, vtargets);
        // queue *myqueue = queue_create(-1);

        graphRun(mygraph, vtargets, targets[k]);

        // graph_destroy(mygraph);
        // vector_destroy(vtargets);
        cleanTargetsStates(mygraph, vtargets);

        k++;
    }

    graph_destroy(mygraph);
    vector_destroy(vtargets);

    return 0;
}

void cleanTargetsStates(graph *theGraph, vector *tgts) {
    size_t size = vector_size(tgts);

    for (size_t i = 0; i < size; i++) {
        rule_t * rule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(tgts, i));
        rule->state = 0;
    }

}


void print_all_targets(graph *theGraph, vector *tgts) {
    size_t size = vector_size(tgts);

    fprintf(stderr, "%zu\n", size);
    for (size_t i = 0; i < size; i++) {
        rule_t * rule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(tgts, i));
        fprintf(stderr, "Node %zu is: %s\n", i, rule->target);
    }
}

int checkCycle(graph *theGraph, vector *tgts, char *goal) {
    size_t size = vector_size(tgts);

    for (size_t i = 0; i < size; i++) {
        rule_t * rule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(tgts, i));
        if (goal == NULL && strlen(rule->target) == 0) {
            void *sentinelVertex = vector_get(tgts, i);
            vector *sentinelsDependency = graph_neighbors(theGraph, sentinelVertex);
            void *goalRule = vector_get(sentinelsDependency, 0);
            vector_destroy(sentinelsDependency);

            if (checkDFS(theGraph, goalRule) == -1) {
                print_cycle_failure(goalRule);
                return -1;
            }
            return 0;
        }
        if (!strcmp(rule->target, goal)) {
            void *goalRule = vector_get(tgts, i);

            if (checkDFS(theGraph, goalRule) == -1) {
                print_cycle_failure(goalRule);
                return -1;
            }

            return 0;
        }
    }
    fprintf(stderr, "No sentinel rules set.\n");
    return -1;
}

int checkDFS(graph *theGraph, void *v) {
    rule_t *rule1 = (rule_t *)graph_get_vertex_value(theGraph, v);
    rule1->state = 1;
    vector *dependencies = graph_neighbors(theGraph, v);
    size_t numberOfDependencies = vector_size(dependencies);
    for (size_t i = 0; i < numberOfDependencies; i++) {
        void *v2 = vector_get(dependencies, i);
        rule_t *rule2 = (rule_t *)graph_get_vertex_value(theGraph, v2);
        if (rule2->state == 0) {
            if (checkDFS(theGraph, v2) == -1) {
                vector_destroy(dependencies);
                return -1;
            }
        } else if (rule2->state == 1) {
            // print_cycle_failure(rule2->target);
            vector_destroy(dependencies);
            return -1;
        }
    }
    rule1->state = 2;

    vector_destroy(dependencies);
    return 0;
}


int graphRun(graph *theGraph, vector *tgts, char *goal) {
    size_t size = vector_size(tgts);

    for (size_t i = 0; i < size; i++) {
        rule_t * rule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(tgts, i));
        if (goal == NULL && strlen(rule->target) == 0) {
            void *sentinelVertex = vector_get(tgts, i);
            vector *sentinelsDependency = graph_neighbors(theGraph, sentinelVertex);
            void *goalRule = vector_get(sentinelsDependency, 0);
            vector_destroy(sentinelsDependency);
            // queue *cmdQueue = queue_create(-1);
            int ret = DFS(theGraph, goalRule);
            // queue_push(theQueue, NULL);

            if (ret == -1) {
                return -1;
            }

            return 0;
        }

        if (!strcmp(rule->target, goal)) {
            void *goalRule = vector_get(tgts, 0);
            // vector_destroy(sentinelsDependency);
            // queue *cmdQueue = queue_create(-1);
            int ret = DFS(theGraph, goalRule);
            // queue_push(theQueue, NULL);

            if (ret == -1) {
                return -1;
            }

            return 0;
        }
    }
    fprintf(stderr, "No sentinel rules set.\n");
    return -1;
}

int DFS(graph *theGraph, void *v) {
    rule_t *rule1 = (rule_t *)graph_get_vertex_value(theGraph, v);
    rule1->state = 1;
    vector *dependencies = graph_neighbors(theGraph, v);
    size_t numberOfDependencies = vector_size(dependencies);

    // FILE *fileOnTheDisk = fopen(rule1->target, "r+");
    // if the curr rule is a file on disk...
    if (access(rule1->target, F_OK) != -1) {
        struct stat myattr, anotherattr;
        int flaggie = 0;
        stat(rule1->target, &myattr);
        for (size_t i = 0; i < numberOfDependencies; i++) {
            void *v2 = vector_get(dependencies, i);
            rule_t *rule2 = (rule_t *)graph_get_vertex_value(theGraph, v2);
            // FILE *anotherFileOnTheDisk = access(rule2->target, F_OK);
            if (access(rule2->target, F_OK) != -1) {
                stat(rule2->target, &anotherattr);
                if (difftime(myattr.st_mtim.tv_sec, anotherattr.st_mtim.tv_sec) < 0.0) {
                    // fclose(anotherFileOnTheDisk);
                    vector_destroy(dependencies);
                    return 0;
                }
            } else {
                // fclose(anotherFileOnTheDisk);
                flaggie = 1;
                break;
            }
            // fclose(anotherFileOnTheDisk);
        }
        if (flaggie == 0) {
            // fclose(fileOnTheDisk);
            vector_destroy(dependencies);
            return 0;
        }

    }
    // fprintf(stderr, "i am here\n");


    for (size_t i = 0; i < numberOfDependencies; i++) {
        void *v2 = vector_get(dependencies, i);
        rule_t *rule2 = (rule_t *)graph_get_vertex_value(theGraph, v2);
        if (rule2->state == 0) {
            if (DFS(theGraph, v2) == -1) {
                rule1->state = 3;
                // vector_destroy(dependencies);
                // return -1;
            }
        } else if (rule2->state == 1) {
            print_cycle_failure(rule2->target);
            vector_destroy(dependencies);
            return -1;
        }
    }
    if (rule1->state == 1) {
        rule1->state = 2;
    }

    if (rule1->state == 2) {
        size_t sizeOfVec = vector_size(rule1->commands);
        for (size_t i = 0; i < sizeOfVec; i++) {
            const char *thisCmd = vector_get(rule1->commands, i);
            int err = system(thisCmd);
            // fprintf(stderr, "err is: %d\n", err);
            if (err != 0) {
                vector_destroy(dependencies);
                return -1;
            }
        }
    }


    vector_destroy(dependencies);
    return 0;
}
