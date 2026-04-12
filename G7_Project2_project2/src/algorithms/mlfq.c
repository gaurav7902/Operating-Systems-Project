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

        /* 1. Calculate time quantum*/
        int quantum;
        if(info[sel].queue_level == 0)
            quantum = TQ_Q0 - info[sel].time_used_in_queue;
        else if(info[sel].queue_level == 1)
            quantum = TQ_Q1 - info[sel].time_used_in_queue;
        else
            quantum = p[sel].remaining_time; 

        if(quantum > p[sel].remaining_time)
            quantum = p[sel].remaining_time;

        /* 2. Execution loop (tick-by-tick part)*/
        int run_time = 0;
        int start_time = current_time;

        while(run_time < quantum && p[sel].remaining_time > 0){
            if(!p[sel].started){
                p[sel].started = 1;
                p[sel].response_time = current_time - p[sel].arrival_time;
            }

            p[sel].remaining_time--;
            current_time++;
            run_time++;
            info[sel].time_used_in_queue++;

            /* stop if priority boost triggers mid-execution*/
            if(current_time % BOOST_INTERVAL == 0)break;

            /*PREEMPTION: check if higher priority process just arrived*/
            if(p[sel].remaining_time > 0 && info[sel].queue_level > 0){
                int preempt = 0;
                for(int i = 0; i < n; i++){
                    if(p[i].remaining_time > 0 && p[i].arrival_time <= current_time && info[i].queue_level < info[sel].queue_level){
                        preempt = 1;
                        break;
                    }
                }
                if(preempt)break;
            }
        }

        /* 3. Recording Gantt chart entry*/
        
        chart[*chart_size].pid = p[sel].pid;
        chart[*chart_size].start = start_time;
        chart[*chart_size].end = current_time;
        (*chart_size)++;
        

        /* 4. Completion or demotion*/
        if(p[sel].remaining_time == 0){
            p[sel].completion_time = current_time;
            completed++;
        }else{
            //process used full quantum => demote to next queue
            int tq_limit = (info[sel].queue_level == 0) ? TQ_Q0 : TQ_Q1;
            if(info[sel].queue_level < 2 && info[sel].time_used_in_queue >= tq_limit){
                info[sel].queue_level++;
                info[sel].time_used_in_queue = 0;
            }
        }

    
    }
}
