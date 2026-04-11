#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "../../include/scheduler_algorithms.h"
#include "../../include/scheduler_types.h"

// System Configuration
#define NUM_CPUS 1      // Multiprocessor setup
#define TIME_QUANTUM 3  // Prevents one process from hogging the CPU forever

// Internal states so we don't modify your Process struct
#define STATE_NOT_ARRIVED 0
#define STATE_READY 1
#define STATE_RUNNING 2
#define STATE_IO 3
#define STATE_FINISHED 4

void runLottery(Process p[], int n, GanttEntry chart[], int *chart_size) {
    int current_time = 0;
    int completed_processes = 0;
    
    int state[MAX_PROCESSES];
    int time_to_next_io[MAX_PROCESSES];
    int current_io_wait[MAX_PROCESSES];
    int current_quantum[MAX_PROCESSES];
    
    int cpu_current_pid[NUM_CPUS];
    int cpu_gantt_start[NUM_CPUS];

    srand((unsigned int)time(NULL));
    *chart_size = 0;

    // Initialize tracking variables
    for (int i = 0; i < n; i++) {
        state[i] = STATE_NOT_ARRIVED;
        time_to_next_io[i] = p[i].io_frequency;
        current_io_wait[i] = 0;
        current_quantum[i] = 0;
    }
    for (int c = 0; c < NUM_CPUS; c++) {
        cpu_current_pid[c] = -1; // -1 = IDLE
    }

    while (completed_processes < n) {
        // 1. Process Arrivals & I/O Completions
        for (int i = 0; i < n; i++) {
            if (state[i] == STATE_NOT_ARRIVED && p[i].arrival_time <= current_time) {
                state[i] = STATE_READY;
            }
            if (state[i] == STATE_IO) {
                current_io_wait[i]--;
                if (current_io_wait[i] <= 0) {
                    state[i] = STATE_READY; // I/O finished
                    time_to_next_io[i] = p[i].io_frequency; // Reset timer
                }
            }
        }

        // 2. Execute processes currently on CPUs
        for (int c = 0; c < NUM_CPUS; c++) {
            int cur_p = cpu_current_pid[c];
            if (cur_p != -1) {
                p[cur_p].remaining_time--;
                current_quantum[cur_p]++;
                if (p[cur_p].io_frequency > 0) time_to_next_io[cur_p]--;

                bool preempt = false;

                // Check if finished
                if (p[cur_p].remaining_time <= 0) {
                    state[cur_p] = STATE_FINISHED;
                    p[cur_p].completion_time = current_time + 1;
                    completed_processes++;
                    preempt = true;
                } 
                // Check if needs I/O
                else if (p[cur_p].io_frequency > 0 && time_to_next_io[cur_p] <= 0) {
                    state[cur_p] = STATE_IO;
                    current_io_wait[cur_p] = p[cur_p].io_time;
                    preempt = true;
                }
                // Check if Quantum Expired (Time slicing for fairness)
                else if (current_quantum[cur_p] >= TIME_QUANTUM) {
                    state[cur_p] = STATE_READY;
                    preempt = true;
                }

                // If preempted, record it in the Gantt chart
                if (preempt) {
                    chart[*chart_size].pid = p[cur_p].pid;
                    chart[*chart_size].start = cpu_gantt_start[c];
                    chart[*chart_size].end = current_time + 1;
                    chart[*chart_size].cpu_id = c; // Note which CPU it was on
                    (*chart_size)++;
                    cpu_current_pid[c] = -1; // Free the CPU
                }
            }
        }

        // 3. The Lottery Draw (Fill Idle CPUs)
        for (int c = 0; c < NUM_CPUS; c++) {
            if (cpu_current_pid[c] == -1) { 
                int total_tickets = 0;
                for (int i = 0; i < n; i++) {
                    if (state[i] == STATE_READY) total_tickets += p[i].tickets;
                }

                if (total_tickets > 0) {
                    int winning_ticket = rand() % total_tickets;
                    int ticket_counter = 0;
                    int winner_idx = -1;

                    // Find the ticket winner
                    for (int i = 0; i < n; i++) {
                        if (state[i] == STATE_READY) {
                            ticket_counter += p[i].tickets;
                            if (ticket_counter > winning_ticket) {
                                winner_idx = i;
                                break;
                            }
                        }
                    }

                    // Assign winner to CPU
                    if (winner_idx != -1) {
                        cpu_current_pid[c] = winner_idx;
                        state[winner_idx] = STATE_RUNNING;
                        current_quantum[winner_idx] = 0;
                        cpu_gantt_start[c] = current_time;

                        // Calculate Response Time (first time it runs)
                        if (!p[winner_idx].started) {
                            p[winner_idx].response_time = current_time - p[winner_idx].arrival_time;
                            p[winner_idx].started = true;
                        }
                    }
                }
            }
        }
        current_time++; // Tick the clock
    }
}