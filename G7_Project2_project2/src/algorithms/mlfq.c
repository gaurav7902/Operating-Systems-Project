#include "../../include/scheduler_algorithms.h"
#include <limits.h>

/* MLFQ Configuration */
#define NUM_QUEUES      3
#define TQ_Q0           4 //fixed time quantum but if want we can change it .
#define TQ_Q1           8
#define BOOST_INTERVAL  20

/* Per-process MLFQ bookkeeping */
typedef struct mlfq_queues{
    int queue_level;
    int time_used_in_queue;
} MLFQInfo;

/*  pick the next process to run */
static int pickProcess(Process p[], int n, MLFQInfo info[], int current_time) {
    for (int q = 0; q < NUM_QUEUES; q++) {
        int best = -1;
        int best_arrival = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (p[i].remaining_time > 0 &&
                p[i].arrival_time <= current_time &&
                info[i].queue_level == q) {

                if (p[i].arrival_time < best_arrival) {
                    best_arrival = p[i].arrival_time;
                    best = i;
                }
            }
        }

        if (best != -1)
            return best;
    }
    return -1;
}

/*  Main MLFQ scheduler */
void runMLFQ(Process p[], int n, GanttEntry chart[], int *chart_size) {
    MLFQInfo info[MAX_PROCESSES];
    int completed = 0;
    int current_time = 0;

    int min_arrival = INT_MAX;
    for (int i = 0; i < n; i++) {
        info[i].queue_level        = 0;
        info[i].time_used_in_queue = 0;
        if (p[i].arrival_time < min_arrival)
            min_arrival = p[i].arrival_time;
    }
    current_time = min_arrival;
    *chart_size = 0;

    while (completed < n) {

        /*  Priority Boost: prevent starvation  */
        if (current_time > 0 && current_time % BOOST_INTERVAL == 0) {
            for (int i = 0; i < n; i++) {
                info[i].queue_level        = 0;
                info[i].time_used_in_queue = 0;
            }
        }

        int sel = pickProcess(p, n, info, current_time);

        /*  IDLE: no process ready */
        if (sel == -1) {
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (p[i].remaining_time > 0 &&
                    p[i].arrival_time > current_time &&
                    p[i].arrival_time < next_arrival)
                    next_arrival = p[i].arrival_time;
            }
            if (next_arrival == INT_MAX) break;

            chart[*chart_size].pid   = -1;
            chart[*chart_size].start = current_time;
            chart[*chart_size].end   = next_arrival;
            (*chart_size)++;
            current_time = next_arrival;
            continue;
        }

    
    }
}
