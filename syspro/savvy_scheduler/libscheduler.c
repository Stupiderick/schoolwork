/**
* Savvy_scheduler Lab
* CS 241 - Fall 2018
*/

// partnered with luo34 and yuyunli2

#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"
typedef struct _job_info {
    int id;
    double arrived_timestamp;
    double start_timestamp;
    double access_timestamp;
    double running_time;
    double time_remained;
    double priority;
    /* Add whatever other bookkeeping you need into this struct. */
} job_info;

priqueue_t pqueue;
scheme_t pqueue_scheme;
comparer_t comparision_func;

static size_t number_finished_jobs;
static double total_wait_time;
static double total_turnaround_time;
static double total_response_time;
static double last_qtimestamp;

void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    number_finished_jobs = 0;
    total_wait_time = 0;
    total_turnaround_time = 0;
    total_response_time = 0;
    last_qtimestamp = -1;
    // Put any set up code you may need here
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
    job *job1 = (job *)a;
    job *job2 = (job *)b;

    job_info *info1 = (job_info *)job1->metadata;
    job_info *info2 = (job_info *)job2->metadata;

    double arrived_time_diff = info2->arrived_timestamp - info1->arrived_timestamp;
    // fprintf(stderr, "fcfs job %d: %f, job %d: %f\n", info1->id, info1->arrived_timestamp, info2->id, info2->arrived_timestamp);

    if (arrived_time_diff > 0) {
        return -1;
    } else if (arrived_time_diff < 0) {
        return 1;
    }

    return 0;
}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
    job *job1 = (job *)a;
    job *job2 = (job *)b;

    job_info *info1 = (job_info *)job1->metadata;
    job_info *info2 = (job_info *)job2->metadata;

    double priority_diff = info2->priority - info1->priority;

    if (priority_diff > 0) {
        return -1;
    } else if (priority_diff < 0) {
        return 1;
    }

    return break_tie(a, b);
}

int comparer_psrtf(const void *a, const void *b) {
    job *job1 = (job *)a;
    job *job2 = (job *)b;

    job_info *info1 = (job_info *)job1->metadata;
    job_info *info2 = (job_info *)job2->metadata;

    double time_remained_diff = info2->time_remained - info1->time_remained;
    // fprintf(stderr, "job %d: %f, job %d: %f\n", info1->id, info1->time_remained, info2->id, info2->time_remained);

    if (time_remained_diff > 0) {
        return -1;
    } else if (time_remained_diff < 0) {
        return 1;
    }

    return break_tie(a, b);
}

int comparer_rr(const void *a, const void *b) {
    job *job1 = (job *)a;
    job *job2 = (job *)b;

    job_info *info1 = (job_info *)job1->metadata;
    job_info *info2 = (job_info *)job2->metadata;

    double access_time_diff = info2->access_timestamp - info1->access_timestamp;

    if (access_time_diff > 0) {
        return -1;
    } else if (access_time_diff < 0) {
        return 1;
    }

    return break_tie(a, b);
}

int comparer_sjf(const void *a, const void *b) {
    job *job1 = (job *)a;
    job *job2 = (job *)b;

    job_info *info1 = (job_info *)job1->metadata;
    job_info *info2 = (job_info *)job2->metadata;

    double running_time_diff = info2->running_time - info1->running_time;

    if (running_time_diff > 0) {
        return -1;
    } else if (running_time_diff < 0) {
        return 1;
    }

    return break_tie(a, b);
}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int number_finished_jobs, double time,
                       scheduler_info *sched_data) {
    // TODO complete me!
    job_info *info = malloc(sizeof(job_info));
    info->arrived_timestamp = time;
    info->start_timestamp = -1;
    info->access_timestamp = -1;
    info->id = number_finished_jobs;
    info->running_time = sched_data->running_time;
    info->time_remained = sched_data->running_time;
    info->priority = sched_data->priority;

    newjob->metadata = (void *)info;
    priqueue_offer(&pqueue, newjob);
}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    // TODO complete me!
    if (last_qtimestamp == -1) {
        last_qtimestamp = time;
    }

    job *next_job;
    if (job_evicted == NULL) {

        if (priqueue_peek(&pqueue) == NULL) {
            return NULL;
        } else {
            next_job = (job*)priqueue_poll(&pqueue);
        }
    } else {
        job_info *info = (job_info *)job_evicted->metadata;
        // double quanta = time - last_qtimestamp;
        info->access_timestamp = time;
        // if (last_qtimestamp == 0) {
        //     info->time_remained = info->start_timestamp - time;
        // }
        info->time_remained -= (time - last_qtimestamp);
        // fprintf(stderr, "quanta: %f\n", time - last_qtimestamp);
        if (pqueue_scheme == PPRI || pqueue_scheme == PSRTF || pqueue_scheme == RR) {
            priqueue_offer(&pqueue, job_evicted);
            next_job = (job *)priqueue_poll(&pqueue);
        } else {
            next_job = job_evicted;
        }
    }

    job_info *next_info = (job_info *)next_job->metadata;
    if (next_info->start_timestamp == -1) {
        next_info->start_timestamp = time;
    }
    last_qtimestamp = time;

    return next_job;
}

void scheduler_job_finished(job *job_done, double time) {
    // TODO complete me!
    job_info *info_done = (job_info *)job_done->metadata;
    info_done->access_timestamp = time;
    total_wait_time += (time - info_done->arrived_timestamp - info_done->running_time);
    total_response_time += (info_done->start_timestamp - info_done->arrived_timestamp);
    total_turnaround_time += (time - info_done->arrived_timestamp);
    free(job_done->metadata);
    number_finished_jobs++;
}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    // TODO complete me!
    return total_wait_time / number_finished_jobs;
}

double scheduler_average_turnaround_time() {
    // TODO complete me!
    return total_turnaround_time / number_finished_jobs;
}

double scheduler_average_response_time() {
    // TODO complete me!
    return total_response_time / number_finished_jobs;
}

void scheduler_show_queue() {
    // Implement this if you need it!
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}
