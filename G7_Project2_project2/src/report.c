#include <stdio.h>
#include "../include/scheduler_report.h"

// Note: Ensure your scheduler_types.h has 'int cpu_id;' in GanttEntry!

void printGantt(GanttEntry chart[], int chart_size) {
    if (chart_size <= 0) {
        printf("\nGantt Chart:\nNo entries\n");
        return;
    }

    printf("\n======================================================\n");
    printf("     VISUALLY MEANINGFUL MULTIPROCESSOR GANTT CHART   \n");
    printf("======================================================\n");

    // Find out how many CPUs were used
    int max_cpu = 0;
    for (int i = 0; i < chart_size; i++) {
        if (chart[i].cpu_id > max_cpu) {
            max_cpu = chart[i].cpu_id;
        }
    }

    // Print a separate timeline for each CPU
    for (int c = 0; c <= max_cpu; c++) {
        printf("CPU %d: ", c);
        for (int i = 0; i < chart_size; i++) {
            if (chart[i].cpu_id == c) {
                if (chart[i].pid == -1) {
                    printf("| IDLE (%d-%d) ", chart[i].start, chart[i].end);
                } else {
                    printf("| P%d (%d-%d) ", chart[i].pid, chart[i].start, chart[i].end);
                }
            }
        }
        printf("|\n");
    }
    printf("======================================================\n");
}

void calculateMetrics(Process p[], int n) {
    float total_tat = 0.0f;
    float total_wt = 0.0f;
    float total_rt = 0.0f;

    printf("\nPID  AT  BT  CT  TAT  WT  RT\n");

    for (int i = 0; i < n; i++) {
        p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
        p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;

        total_tat += (float)p[i].turnaround_time;
        total_wt += (float)p[i].waiting_time;
        total_rt += (float)p[i].response_time;

        printf("P%d   %d   %d   %d   %d    %d   %d\n",
               p[i].pid,
               p[i].arrival_time,
               p[i].burst_time,
               p[i].completion_time,
               p[i].turnaround_time,
               p[i].waiting_time,
               p[i].response_time);
    }

    printf("\nAverage TAT = %.2f\n", total_tat / (float)n);
    printf("Average WT  = %.2f\n", total_wt / (float)n);
    printf("Average RT  = %.2f\n", total_rt / (float)n);
}