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
#include <pthread.h>

#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "queue.h"
#include "set.h"

typedef struct bundle {
    int pid;
    graph *theGraph;
    vector *allTargets;
    vector *goodRuleVector;
} bundle;

set *runningRules;
set *satisfiedRules;
set *failedRules;
set *runRules;
int goodToRun;
int numOfRulesRun;
// graph *staticgraph;
pthread_cond_t cv;
pthread_mutex_t enterlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t failedrulelock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t satisfiedrulelock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t icanrunlock = PTHREAD_MUTEX_INITIALIZER;


void print_all_targets(graph *theGraph, vector *allTargets);
void print_vec(vector *vec);
int checkCycle(graph *theGraph, vector *allTargets, char *goal, set *activeSet, set *finishedSet, queue *goodRuleQueue);
int checkDFS(graph *theGraph, void *v, set *activeSet, set *finishedSet, queue *goodRuleQueue);
void *runCmd(void *thebundle);
size_t findTarget(graph *theGraph, vector *allTargets, char *theRule);


int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    graph *mygraph = parser_parse_makefile(makefile, targets);
    vector *vtargets = graph_vertices(mygraph);
    int k = 0;
    set *activeSet = string_set_create();
    set *finishedSet = string_set_create();
    vector *goodRuleVector = string_vector_create();
    queue *goodRuleQueue = queue_create(-1);
    set *goodRuleSet = string_set_create();

    runningRules = string_set_create();
    satisfiedRules = string_set_create();
    failedRules = string_set_create();
    runRules = string_set_create();

    // int short_ids[num_threads];
    pthread_t ids[num_threads];

    // when there is no goal rule specified, find sentinel...
    if (targets[0] == NULL) {
        // if the rule does not fail, then we can try to push them into the vec...
        if (checkCycle(mygraph, vtargets, NULL, activeSet, finishedSet, goodRuleQueue) != -1) {
            while (1) {
                char *tar = queue_pull(goodRuleQueue);
                if (tar == NULL) {
                    break;
                }
                if (!set_contains(goodRuleSet, tar)) {
                    set_add(goodRuleSet, tar);
                    vector_push_back(goodRuleVector, tar);
                }
            }
        }

        // print_vec(goodRuleVector);
        bundle **mybundle = malloc(num_threads * sizeof(bundle *));
        for (size_t e = 0; e < num_threads; e++) {
            mybundle[e] = malloc(sizeof(bundle));
            mybundle[e]->theGraph = mygraph;
            mybundle[e]->allTargets = vtargets;
            mybundle[e]->goodRuleVector = goodRuleVector;
            mybundle[e]->pid = e + 1;
        }

        // print_vec(goodRuleVector);

        pthread_cond_init(&cv, NULL);

        for (size_t i = 0; i < num_threads; i++) {
            // short_ids[i] = i + 1;
            pthread_create(&ids[i], NULL, runCmd, mybundle[i]);
        }

        for (size_t i = 0; i < num_threads; i++) {
            pthread_join(ids[i], NULL);
        }


        // runCmd(mybundle);

        // run cmds with default goal rule...
        //pushCmdToVector(mygraph, vtargets, NULL);
        for (size_t e = 0; e < num_threads; e++) {
            free(mybundle[e]);
        }
        free(mybundle);
        graph_destroy(mygraph);
        vector_destroy(vtargets);
        set_destroy(activeSet);
        set_destroy(finishedSet);
        vector_destroy(goodRuleVector);
        queue_destroy(goodRuleQueue);
        set_destroy(goodRuleSet);
        pthread_cond_destroy(&cv);
        set_destroy(runningRules);
        set_destroy(satisfiedRules);
        set_destroy(failedRules);
        set_destroy(runRules);

        return 0;
    }

    // Or we have specified the goal rules we want...
    while (1) {
        // when we reach the end...
        if (targets[k] == NULL) {
            break;
        }

        // If this rule has cycles, we continues...
        if (checkCycle(mygraph, vtargets, targets[k], activeSet, finishedSet, goodRuleQueue) != -1) {
            // vector_push_back(goodRuleVector, targets[k]);
            while (1) {
                char *tar = queue_pull(goodRuleQueue);
                // fprintf(stderr, "tar is: %s. and set: %d\n", tar, set_contains(goodRuleSet, tar));
                if (tar == NULL) {
                    break;
                }
                if (!set_contains(goodRuleSet, tar)) {
                    set_add(goodRuleSet, tar);
                    vector_push_back(goodRuleVector, tar);
                }
            }
        }

        k++;
        set_clear(activeSet);
        set_clear(finishedSet);
    }

    bundle **mybundle = malloc(num_threads * sizeof(bundle *));
    for (size_t e = 0; e < num_threads; e++) {
        mybundle[e] = malloc(sizeof(bundle));
        mybundle[e]->theGraph = mygraph;
        mybundle[e]->allTargets = vtargets;
        mybundle[e]->goodRuleVector = goodRuleVector;
        mybundle[e]->pid = e + 1;
    }

    // print_vec(goodRuleVector);

    pthread_cond_init(&cv, NULL);

    for (size_t i = 0; i < num_threads; i++) {
        // short_ids[i] = i + 1;
        pthread_create(&ids[i], NULL, runCmd, mybundle[i]);
    }

    for (size_t i = 0; i < num_threads; i++) {
        pthread_join(ids[i], NULL);
    }

    // runCmd(mybundle);
    // set *rulesChecked = string_set_create();
    // size_t goodRuleSize = vector_size(goodRuleVector);
    // for (size_t i = 0; i < goodRuleSize; i++) {
    //     //pushCmdToVector(mygraph, vtargets, (char *)vector_get(goodRuleVector, i), rulesChecked);
    // }

    // print_vec(goodRuleVector);
    for (size_t e = 0; e < num_threads; e++) {
        free(mybundle[e]);
    }
    free(mybundle);
    graph_destroy(mygraph);
    vector_destroy(vtargets);
    set_destroy(activeSet);
    set_destroy(finishedSet);
    vector_destroy(goodRuleVector);
    queue_destroy(goodRuleQueue);
    set_destroy(goodRuleSet);
    pthread_cond_destroy(&cv);
    set_destroy(runningRules);
    set_destroy(satisfiedRules);
    set_destroy(failedRules);
    set_destroy(runRules);


    return 0;
}

void print_vec(vector *vec) {
    size_t size = vector_size(vec);

    for (size_t i = 0; i < size; i++) {
        fprintf(stderr, "rule %zu is: %s\n", i, (char *)vector_get(vec, i));
    }
}

void print_all_targets(graph *theGraph, vector *allTargets) {
    size_t size = vector_size(allTargets);

    fprintf(stderr, "%zu\n", size);
    for (size_t i = 0; i < size; i++) {
        rule_t * rule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(allTargets, i));
        fprintf(stderr, "Node %zu is: %s\n", i, rule->target);
    }
}

int checkCycle(graph *theGraph, vector *allTargets, char *goal, set *activeSet, set *finishedSet, queue *goodRuleQueue) {
    // fprintf(stderr, "this is goal: %s\n", goal);
    size_t numOfTargets = vector_size(allTargets);
    // fprintf(stderr, "my goal is: %s\n", goal);

    // check which rule is goal rule
    for (size_t i = 0; i < numOfTargets; i++) {
        rule_t *rule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(allTargets, i));

        // when goal rule is not specified from arguments, find the sentinel
        if (goal == NULL && strlen(rule->target) == 0) {
            void *sentinelVertex = vector_get(allTargets, i);
            vector *sentinelsDependency = graph_neighbors(theGraph, sentinelVertex);
            void *goalRule = vector_get(sentinelsDependency, 0);
            vector_destroy(sentinelsDependency);

            if (checkDFS(theGraph, goalRule, activeSet, finishedSet, goodRuleQueue) == -1) {
                print_cycle_failure(((rule_t *)graph_get_vertex_value(theGraph, goalRule))->target);
                queue_push(goodRuleQueue, NULL);
                return -1;
            }
            queue_push(goodRuleQueue, NULL);
            return 0;
        }
        if (!strcmp(rule->target, goal)) {
            void *goalRule = vector_get(allTargets, i);

            if (checkDFS(theGraph, goalRule, activeSet, finishedSet, goodRuleQueue) == -1) {
                print_cycle_failure(goal);
                queue_push(goodRuleQueue, NULL);
                return -1;
            }
            queue_push(goodRuleQueue, NULL);
            return 0;
        }
    }
    fprintf(stderr, "No sentinel rules set.\n");
    return -1;
}

int checkDFS(graph *theGraph, void *v, set *activeSet, set *finishedSet, queue *goodRuleQueue) {
    rule_t *rule1 = (rule_t *)graph_get_vertex_value(theGraph, v);
    set_add(activeSet, rule1->target);

    vector *dependencies = graph_neighbors(theGraph, v);
    size_t numberOfDependencies = vector_size(dependencies);
    for (size_t i = 0; i < numberOfDependencies; i++) {
        void *v2 = vector_get(dependencies, i);
        rule_t *rule2 = (rule_t *)graph_get_vertex_value(theGraph, v2);
        if (!set_contains(activeSet, rule2->target) && !set_contains(finishedSet, rule2->target)) {
            if (checkDFS(theGraph, v2, activeSet, finishedSet, goodRuleQueue) == -1) {
                vector_destroy(dependencies);
                return -1;
            }
        } else if (set_contains(activeSet, rule2->target)) {
            vector_destroy(dependencies);
            return -1;
        }
    }
    set_remove(activeSet, rule1->target);
    set_add(finishedSet, rule1->target);
    queue_push(goodRuleQueue, rule1->target);

    vector_destroy(dependencies);
    return 0;
}




size_t findTarget(graph *theGraph, vector *allTargets, char *theRule) {
    size_t numOfTargets = vector_size(allTargets);
    for (size_t j = 0; j < numOfTargets; j++) {
        rule_t *rule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(allTargets, j));

        if (!strcmp(rule->target, theRule)) {
            return j;
        }
    }

    return 100;
}


void *runCmd(void *thebundle) {
    bundle *mybundle = thebundle;
    graph *theGraph = mybundle->theGraph;
    vector *allTargets = mybundle->allTargets;
    vector *goodRuleVector = mybundle->goodRuleVector;
    size_t numOfGoodRules = vector_size(goodRuleVector);

    while (1) {
        // fprintf(stderr, "here %d\n", mybundle->pid);
        // traverse to check each elem in goodRuleVec...
        for (size_t i = 0; i < numOfGoodRules; i++) {
            // get target idx...
            size_t idxTar = findTarget(theGraph, allTargets, (char *)vector_get(goodRuleVector, i));
            rule_t *targetRule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(allTargets, idxTar));
            // if the rule is in use or used, continue;
            pthread_mutex_lock(&enterlock);
            if (set_contains(runningRules, targetRule->target) || set_contains(runRules, targetRule->target)) {
                pthread_mutex_unlock(&enterlock);
                continue;
            } else {
                set_add(runningRules, targetRule->target);
            }
            pthread_mutex_unlock(&enterlock);

            // get its dependencies...
            vector *dependencies = graph_neighbors(theGraph, vector_get(allTargets, idxTar));
            size_t numberOfDependencies = vector_size(dependencies);
            int flag = 1;

            // check each dependency...
            for (size_t k = 0; k < numberOfDependencies; k++) {
                size_t idxDep = findTarget(theGraph, allTargets, vector_get(dependencies, k));
                rule_t *rule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(allTargets, idxDep));
                // fprintf(stderr, "%s depends on %s\n", targetRule->target, rule->target);

                pthread_mutex_lock(&enterlock);
                // if one of the rule dependency is not run... we set aside this rule...
                if (!set_contains(runRules, rule->target)) {
                    pthread_mutex_unlock(&enterlock);
                    // indicate that this rule should keep waiting... because its dependencies are still not touched...
                    flag = 0;
                    vector_destroy(dependencies);
                    break;
                }
                pthread_mutex_unlock(&enterlock);


                pthread_mutex_lock(&failedrulelock);
                // else we check if the dependency is failed...
                if (set_contains(failedRules, rule->target)) {
                    pthread_mutex_unlock(&failedrulelock);
                    flag = 2;
                    vector_destroy(dependencies);
                    break;
                }
                pthread_mutex_unlock(&failedrulelock);
            }

            // if the rule's dependency is not finished... find the next rule...
            if (flag == 0) {
                pthread_mutex_lock(&enterlock);
                set_remove(runningRules, targetRule->target);
                pthread_mutex_unlock(&enterlock);
                // fprintf(stderr, "oops\n");
                continue;
                // return NULL;
                // TODO when we should wait...
            } else if (flag == 1) {
                // if the rule can be run...
                int allFiles = 1;
                int hasNewFile = 0;
                // check file on the disk, if it is...
                if (access(targetRule->target, F_OK) != -1) {
                    struct stat myattr;
                    if (stat(targetRule->target, &myattr) != 0) return NULL;
                    for (size_t p = 0; p < numberOfDependencies; p++) {
                        size_t idxDep = findTarget(theGraph, allTargets, vector_get(dependencies, p));
                        rule_t *rule = (rule_t *)graph_get_vertex_value(theGraph, vector_get(allTargets, idxDep));

                        // when there is a dependency that is not file...
                        if (access(rule->target, F_OK) == -1) {
                            allFiles = 0;
                            // vector_destroy(dependencies);
                        } else {
                            struct stat depenattr;
                            if (stat(rule->target, &depenattr) != 0) return NULL;
                            // when there is a file that is newer than the rule...
                            if (difftime(depenattr.st_mtime, myattr.st_mtime) > 0.0) {
                                hasNewFile = 1;
                            }
                        }
                        if (!allFiles || hasNewFile) {
                            break;
                        }
                    }

                    // when all of the dependencies are files and no new files...
                    if (allFiles && !hasNewFile) {
                        pthread_mutex_lock(&satisfiedrulelock);
                        set_add(satisfiedRules, targetRule->target);
                        pthread_mutex_unlock(&satisfiedrulelock);
                        pthread_mutex_lock(&enterlock);
                        set_remove(runningRules, targetRule->target);
                        set_add(runRules, targetRule->target);
                        pthread_mutex_unlock(&enterlock);
                        pthread_cond_broadcast(&cv);
                        pthread_mutex_lock(&icanrunlock);
                        goodToRun = 1;
                        pthread_mutex_unlock(&icanrunlock);
                        // targetRule->state = 1;
                        vector_destroy(dependencies);
                        continue;
                        // return NULL;
                    }
                }
                vector_destroy(dependencies);
                // if it reaches here, then we can try to satisfy the exec...
                int allRan = 1;
                size_t numOfCmd = vector_size(targetRule->commands);
                for (size_t t = 0; t < numOfCmd; t++) {
                    const char *thisCmd = vector_get(targetRule->commands, t);
                    // fprintf(stderr, "cmd is: %s\n", thisCmd);
                    int err = system(thisCmd);
                    // fprintf(stderr, "blaaa%d\n", mybundle->pid);
                    if (err != 0) {
                        // if the cmd is failed...no more run and this rule is marked by failed..
                        pthread_mutex_lock(&failedrulelock);
                        set_add(failedRules, targetRule->target);
                        pthread_mutex_unlock(&failedrulelock);
                        pthread_mutex_lock(&enterlock);
                        set_remove(runningRules, targetRule->target);
                        set_add(runRules, targetRule->target);
                        pthread_mutex_unlock(&enterlock);
                        pthread_cond_broadcast(&cv);
                        pthread_mutex_lock(&icanrunlock);
                        goodToRun = 1;
                        pthread_mutex_unlock(&icanrunlock);
                        allRan = 0;
                        break;
                    }
                }

                // if all the cmds ran, mark it as satisfied...
                if (allRan) {
                    pthread_mutex_lock(&satisfiedrulelock);
                    set_add(satisfiedRules, targetRule->target);
                    pthread_mutex_unlock(&satisfiedrulelock);
                    // fprintf(stderr, "blaaaaaaaaa %d\n", mybundle->pid);
                    pthread_mutex_lock(&enterlock);
                    // fprintf(stderr, "blaaa %d\n", mybundle->pid);
                    set_remove(runningRules, targetRule->target);
                    set_add(runRules, targetRule->target);
                    pthread_mutex_unlock(&enterlock);

                    pthread_cond_broadcast(&cv);
                    pthread_mutex_lock(&icanrunlock);
                    goodToRun = 1;
                    pthread_mutex_unlock(&icanrunlock);
                }


            } else {
                // indicate that this rule is failed because of the failure of its dependencies...
                pthread_mutex_lock(&failedrulelock);
                set_add(failedRules, targetRule->target);
                pthread_mutex_unlock(&failedrulelock);
                pthread_mutex_lock(&enterlock);
                set_remove(runningRules, targetRule->target);
                set_add(runRules, targetRule->target);
                pthread_mutex_unlock(&enterlock);
                pthread_cond_broadcast(&cv);
                pthread_mutex_lock(&icanrunlock);
                goodToRun = 1;
                pthread_mutex_unlock(&icanrunlock);
            }
        }
        // fprintf(stderr, "reached%d %d\n", mybundle->pid, goodToRun);

        pthread_mutex_lock(&enterlock);

        if (set_cardinality(runRules) == numOfGoodRules) {
            pthread_mutex_unlock(&enterlock);
            pthread_cond_broadcast(&cv);
            // fprintf(stderr, "reached again%d %d\n", mybundle->pid, goodToRun);
            return NULL;
        } else {
            pthread_mutex_unlock(&enterlock);

            pthread_mutex_lock(&icanrunlock);
            // pthread_mutex_lock(&icanrunlock);
            goodToRun = 0;
            // pthread_mutex_unlock(&icanrunlock);

            while (goodToRun == 0) {
                // fprintf(stderr, "reached wait%d %d\n", mybundle->pid, goodToRun);
                pthread_cond_wait(&cv, &icanrunlock);

                pthread_mutex_lock(&enterlock);
                // fprintf(stderr, "reached waked%d %d %zu %zu\n", mybundle->pid, goodToRun, set_cardinality(runRules), numOfGoodRules);
                if (set_cardinality(runRules) == numOfGoodRules) {
                    pthread_mutex_unlock(&enterlock);
                    pthread_mutex_unlock(&icanrunlock);
                    pthread_cond_broadcast(&cv);
                    // fprintf(stderr, "reached again%d %d\n", mybundle->pid, goodToRun);
                    return NULL;
                }

                pthread_mutex_unlock(&enterlock);
            }

            pthread_mutex_unlock(&icanrunlock);
        }


    }
    pthread_cond_broadcast(&cv);
    // fprintf(stderr, "reached again%d %d\n", mybundle->pid, goodToRun);
    return NULL;
}
