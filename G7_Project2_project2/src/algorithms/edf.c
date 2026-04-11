#include "../../include/scheduler_algorithms.h"
#include <limits.h>

void runEDF(Process p[], int n, GanttEntry chart[], int *chart_size) {
    int current_time = 0;
    int completed = 0;

    /* Find the earliest arrival time to start scheduling */
    int min_arrival = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time < min_arrival) {
            min_arrival = p[i].arrival_time;
        }
    }

    current_time = min_arrival;

    while (completed < n) {
        /* Find process with earliest deadline among those who have arrived */
        int selected = -1;
        int earliest_deadline = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (p[i].remaining_time > 0 && p[i].arrival_time <= current_time) {
                if (p[i].deadline < earliest_deadline) {
                    earliest_deadline = p[i].deadline;
                    selected = i;
                }
            }
        }

        if (selected == -1) {
            /* No process has arrived yet, fast forward to next arrival */
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (p[i].remaining_time > 0 && p[i].arrival_time > current_time) {
                    if (p[i].arrival_time < next_arrival) {
                        next_arrival = p[i].arrival_time;
                    }
                }
            }

            if (next_arrival == INT_MAX) {
                break; /* No more processes */
            }

            /* Add IDLE entry */
            chart[*chart_size].pid = -1;
            chart[*chart_size].start = current_time;
            chart[*chart_size].end = next_arrival;
            (*chart_size)++;
            current_time = next_arrival;
            continue;
        }

        
    }
}
