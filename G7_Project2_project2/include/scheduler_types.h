#ifndef SCHEDULER_TYPES_H
#define SCHEDULER_TYPES_H

#include <stdbool.h>

#define MAX_PROCESSES 100
#define MAX_GANTT_ENTRIES 1000

typedef struct {
    int pid;

    int arrival_time;
    int burst_time;
    int remaining_time;

    int io_time;
    int io_frequency;

    int priority;
    int deadline;
    int tickets;

    int completion_time;
    int turnaround_time;
    int waiting_time;

    int last_executed_time;
    int response_time;
    bool started;
} Process;

typedef struct {
    int pid; /* -1 for IDLE */
    int start;
    int end;
} GanttEntry;

#endif
