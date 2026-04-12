#include <stdio.h>

#include "include/scheduler_algorithms.h"
#include "include/scheduler_input.h"
#include "include/scheduler_report.h"
#include "include/scheduler_types.h"

int main(void)
{
    Process p[MAX_PROCESSES];
    GanttEntry chart[MAX_GANTT_ENTRIES];
    int n = 0;
    int choice;
    int input_mode;
    int chart_size = 0;

    input_mode = chooseInputMode();
    if (input_mode == 1)
    {
        printf("Enter number of processes: ");
        scanf("%d", &n);

        if (n <= 0 || n > MAX_PROCESSES)
        {
            printf("Invalid number of processes. Range: 1 to %d\n", MAX_PROCESSES);
            return 0;
        }

        readProcesses(p, n);
    }
    else if (input_mode == 2)
    {
        if (!loadDummyProcesses(p, &n))
        {
            return 0;
        }
        printProcessData(p, n);
    }
    else
    {
        printf("Invalid input mode\n");
        return 0;
    }

    choice = chooseAlgorithm();

    if (choice == 1)
        runEDF(p, n, chart, &chart_size);
    else if (choice == 2)
        runLottery(p, n, chart, &chart_size);
    else if (choice == 3)
        runMLFQ(p, n, chart, &chart_size);
    else
    {
        printf("Invalid choice\n");
        return 0;
    }

    printGantt(chart, chart_size);
    calculateMetrics(p, n);

    return 0;
}