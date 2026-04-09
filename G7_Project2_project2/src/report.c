#include <stdio.h>

#include "../include/scheduler_report.h"

void printGantt(GanttEntry chart[], int chart_size) {
    if (chart_size <= 0) {
        printf("\nGantt Chart:\nNo entries\n");
        return;
    }

    printf("\nGantt Chart:\n");

    for (int i = 0; i < chart_size; i++) {
        if (chart[i].pid == -1)
            printf("| IDLE ");
        else
            printf("| P%d ", chart[i].pid);
    }
    printf("|\n");

    for (int i = 0; i < chart_size; i++) {
        printf("%d    ", chart[i].start);
    }
    printf("%d\n", chart[chart_size - 1].end);
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
