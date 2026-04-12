#include <stdio.h>

#include "include/scheduler_algorithms.h"
#include "include/scheduler_report.h"
#include "include/scheduler_types.h"

static void readProcesses(Process p[], int n) {
    for (int i = 0; i < n; i++) {
        printf("\nProcess %d:\n", i + 1);

        p[i].pid = i + 1;

        printf("Arrival Time: ");
        scanf("%d", &p[i].arrival_time);

        printf("Burst Time: ");
        scanf("%d", &p[i].burst_time);

        printf("IO Time: ");
        scanf("%d", &p[i].io_time);

        printf("IO Frequency: ");
        scanf("%d", &p[i].io_frequency);

        printf("Priority: ");
        scanf("%d", &p[i].priority);

        printf("Deadline: ");
        scanf("%d", &p[i].deadline);

        printf("Tickets: ");
        scanf("%d", &p[i].tickets);

        p[i].remaining_time = p[i].burst_time;
        p[i].completion_time = 0;
        p[i].turnaround_time = 0;
        p[i].waiting_time = 0;
        p[i].last_executed_time = 0;
        p[i].response_time = 0;
        p[i].started = false;
    }
}

static int chooseAlgorithm(void) {
    int choice;

    printf("\nChoose Algorithm:\n");
    printf("1. EDF\n2. Lottery\n3. MLFQ\n");
    scanf("%d", &choice);

    return choice;
}

int main(void) {
    Process p[MAX_PROCESSES];
    GanttEntry chart[MAX_GANTT_ENTRIES];
    int n;
    int choice;
    int chart_size = 0;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    if (n <= 0 || n > MAX_PROCESSES) {
        printf("Invalid number of processes. Range: 1 to %d\n", MAX_PROCESSES);
        return 0;
    }

    readProcesses(p, n);
    choice = chooseAlgorithm();

    if (choice == 1)
        runEDF(p, n, chart, &chart_size);
    else if (choice == 2)
        runLottery(p, n, chart, &chart_size);
    else if (choice == 3)
        runMLFQ(p, n, chart, &chart_size);
    else {
        printf("Invalid choice\n");
        return 0;
    }

    printGantt(chart, chart_size);
    calculateMetrics(p, n);

    return 0;
}
