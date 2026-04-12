#include <stdio.h>

#include "../include/scheduler_input.h"

#define DUMMY_DATA_FILE "example_processes_data.txt"

static void initializeProcess(Process *proc,
                              int pid,
                              int arrival_time,
                              int burst_time,
                              int io_time,
                              int io_frequency,
                              int priority,
                              int deadline,
                              int tickets)
{
    proc->pid = pid;
    proc->arrival_time = arrival_time;
    proc->burst_time = burst_time;
    proc->io_time = io_time;
    proc->io_frequency = io_frequency;
    proc->priority = priority;
    proc->deadline = deadline;
    proc->tickets = tickets;

    proc->remaining_time = proc->burst_time;
    proc->completion_time = 0;
    proc->turnaround_time = 0;
    proc->waiting_time = 0;
    proc->last_executed_time = 0;
    proc->response_time = 0;
    proc->started = false;
}

void readProcesses(Process p[], int n)
{
    for (int i = 0; i < n; i++)
    {
        int arrival_time;
        int burst_time;
        int io_time;
        int io_frequency;
        int priority;
        int deadline;
        int tickets;

        printf("\nProcess %d:\n", i + 1);

        printf("Arrival Time: ");
        scanf("%d", &arrival_time);

        printf("Burst Time: ");
        scanf("%d", &burst_time);

        printf("IO Time: ");
        scanf("%d", &io_time);

        printf("IO Frequency: ");
        scanf("%d", &io_frequency);

        printf("Priority: ");
        scanf("%d", &priority);

        printf("Deadline: ");
        scanf("%d", &deadline);

        printf("Tickets: ");
        scanf("%d", &tickets);

        initializeProcess(&p[i],
                          i + 1,
                          arrival_time,
                          burst_time,
                          io_time,
                          io_frequency,
                          priority,
                          deadline,
                          tickets);
    }
}

int loadDummyProcesses(Process p[], int *n)
{
    FILE *fp = fopen(DUMMY_DATA_FILE, "r");
    if (fp == NULL)
    {
        printf("Could not open %s\n", DUMMY_DATA_FILE);
        return 0;
    }

    if (fscanf(fp, "%d", n) != 1)
    {
        printf("Invalid dummy data file format.\n");
        fclose(fp);
        return 0;
    }

    if (*n <= 0 || *n > MAX_PROCESSES)
    {
        printf("Invalid process count in %s. Range: 1 to %d\n", DUMMY_DATA_FILE, MAX_PROCESSES);
        fclose(fp);
        return 0;
    }

    for (int i = 0; i < *n; i++)
    {
        int arrival_time;
        int burst_time;
        int io_time;
        int io_frequency;
        int priority;
        int deadline;
        int tickets;

        if (fscanf(fp,
                   "%d %d %d %d %d %d %d",
                   &arrival_time,
                   &burst_time,
                   &io_time,
                   &io_frequency,
                   &priority,
                   &deadline,
                   &tickets) != 7)
        {
            printf("Invalid process row at line %d in %s\n", i + 2, DUMMY_DATA_FILE);
            fclose(fp);
            return 0;
        }

        initializeProcess(&p[i],
                          i + 1,
                          arrival_time,
                          burst_time,
                          io_time,
                          io_frequency,
                          priority,
                          deadline,
                          tickets);
    }

    fclose(fp);
    return 1;
}

void printProcessData(const Process p[], int n)
{
    printf("\nLoaded dummy process data:\n");
    printf("PID  ARRIVAL  BURST  IO_TIME  IO_FREQ  PRIORITY  DEADLINE  TICKETS\n");
    for (int i = 0; i < n; i++)
    {
        printf("%-4d %-8d %-6d %-8d %-8d %-9d %-9d %-7d\n",
               p[i].pid,
               p[i].arrival_time,
               p[i].burst_time,
               p[i].io_time,
               p[i].io_frequency,
               p[i].priority,
               p[i].deadline,
               p[i].tickets);
    }
}

int chooseInputMode(void)
{
    int mode;

    printf("\nChoose Process Input Mode:\n");
    printf("1. Enter process data manually\n");
    printf("2. Use dummy data from %s\n", DUMMY_DATA_FILE);
    scanf("%d", &mode);

    return mode;
}

int chooseAlgorithm(void)
{
    int choice;

    printf("\nChoose Algorithm:\n");
    printf("1. EDF\n2. Lottery\n3. MLFQ\n");
    scanf("%d", &choice);

    return choice;
}
