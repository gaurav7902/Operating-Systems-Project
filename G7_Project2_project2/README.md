# Project 2 Scheduler

Final implementation of a Process Scheduling Simulator in C with three algorithms:
- Earliest Deadline First (EDF)
- Lottery Scheduling
- Multi-Level Feedback Queue (MLFQ)

The program accepts process input either manually or from a dummy data file, runs the selected scheduler, prints a Gantt chart, and reports key timing metrics.

## Project Objective

This project simulates CPU scheduling behavior and compares different scheduling strategies using a shared process model. It is designed to demonstrate:
- Scheduler selection and dispatch from a single main program flow.
- Fairness and responsiveness trade-offs between algorithms.
- Measurable outcomes through completion, turnaround, waiting, and response times.

## Features

- Manual process data input.
- Dummy dataset loading from a text file.
- Runtime scheduler selection (EDF / Lottery / MLFQ).
- Gantt chart generation.
- Per-process and aggregate metric reporting.

## Current Structure

```text
G7_Project2_project2/
├── Makefile
├── README.md
├── main.c
├── example_processes_data.txt
├── include/
│   ├── scheduler_types.h
│   ├── scheduler_report.h
│   ├── scheduler_algorithms.h
│   └── scheduler_input.h
└── src/
    ├── input.c
    ├── report.c
    └── algorithms/
        ├── edf.c
        ├── lottery.c
        └── mlfq.c
```

## Core Data Model

Each process stores:
- Identity: pid
- Arrival and CPU burst data: arrival_time, burst_time, remaining_time
- I/O behavior: io_time, io_frequency
- Scheduler-specific attributes: priority, deadline, tickets
- Metrics fields: completion_time, turnaround_time, waiting_time, response_time, started

Gantt chart entries store:
- pid (or -1 for idle)
- start time and end time
- cpu_id (used by Lottery implementation)

## Input Flow

When the program starts, it prompts for process input mode:

1. Manual mode
- You enter number of processes and all attributes for each process.

2. Dummy mode
- The app reads process data from example_processes_data.txt.
- The loaded process table is printed before scheduling starts.

After input is prepared, the user chooses one algorithm (EDF, Lottery, or MLFQ), and the simulator executes that algorithm on the same process model.

## Dummy File Format

File: example_processes_data.txt

- First line: number of processes N.
- Next N lines: one process per line in this order:

```text
arrival_time burst_time io_time io_frequency priority deadline tickets
```

Example:

```text
5
0 8 0 0 3 18 20
1 5 2 2 1 12 10
2 7 1 3 2 16 25
3 4 0 0 4 10 15
4 6 2 2 2 14 30
```

## Implementation Details

### 1. EDF (Earliest Deadline First)

File: src/algorithms/edf.c

How it is implemented:
- The scheduler starts from the minimum arrival time.
- At each time unit, among all arrived and unfinished processes, it picks the one with the smallest deadline.
- If no process is ready, it inserts an IDLE segment and jumps to the next arrival.
- Execution is preemptive at 1-time-unit granularity.
- Response time is set when a process runs for the first time.
- Completion time is set when remaining_time becomes zero.

Why this matches EDF:
- Selection is strictly based on earliest absolute deadline among ready processes.
- Re-evaluation occurs every tick, so newly arrived earlier-deadline tasks can preempt.

### 2. Lottery Scheduling

File: src/algorithms/lottery.c

How it is implemented:
- Ready processes contribute tickets to a global ticket pool.
- A random winning ticket is drawn; the corresponding process gets CPU.
- A fixed time quantum is used to prevent CPU monopolization.
- Processes can move to I/O state based on io_frequency and io_time, then return to READY.
- Completion, response, and Gantt segments are updated during preemption/finish transitions.

Why this matches Lottery scheduling:
- CPU allocation probability is proportional to ticket count.
- Time slicing and state transitions maintain practical fairness and realism.

### 3. MLFQ (Multi-Level Feedback Queue)

File: src/algorithms/mlfq.c

How it is implemented:
- Three priority queues are used.
- Higher-priority queues use shorter quanta; lower queues use longer service windows.
- New processes begin at the highest priority queue.
- If a process exhausts its queue quantum, it is demoted.
- Periodic priority boost moves all runnable processes back to top priority to reduce starvation.
- Preemption occurs when a higher-priority process becomes available.

Why this matches MLFQ:
- Dynamic priority adjustment is based on observed CPU usage.
- Interactive/short jobs are favored while starvation is mitigated by boosting.

## Reporting and Metrics

Files:
- src/report.c
- include/scheduler_report.h

The simulator prints:
- Gantt chart timeline.
- Per-process metrics (completion, turnaround, waiting, response).
- Overall behavior can be analyzed by comparing outputs across algorithms using the same input set.

## Prototype Rules

- Keep scheduling algorithm prototypes in include/scheduler_algorithms.h.
- Keep input-related prototypes in include/scheduler_input.h.
- If you add a new scheduling algorithm:
- Add its prototype in include/scheduler_algorithms.h.
- Add its source file in src/algorithms/.
- Add selection logic in main.c.
- Add the new source path to Makefile under SOURCES.

## Shared Files

- include/scheduler_types.h: shared Process and GanttEntry structs.
- include/scheduler_report.h and src/report.c: Gantt printing and metrics logic.
- include/scheduler_input.h and src/input.c: process input and mode handling.
- main.c: overall program flow and algorithm dispatch.

## Build and Run

From this folder:

```bash
make
make run
make clean
```

## How To Demonstrate For Evaluation

Suggested marking/demo flow:
1. Run with dummy data and choose EDF. Show Gantt chart and metrics.
2. Run again with the same dummy data and choose Lottery. Compare behavior.
3. Run again with the same dummy data and choose MLFQ. Compare response/waiting trends.
4. Run in manual mode with a custom test case to show flexibility.

This demonstrates both correctness and comparative analysis of scheduling policies.

## Contributors and References

| Algorithm | Contributor | GitHub |
|---|---|---|
| EDF | John Gunji | https://github.com/johngunji |
| EDF | Gaurav Patidar | https://github.com/gaurav7902 |
| MLFQ | Ishika Acharya | https://github.com/mockingjay777 |
| MLFQ | Sathish | https://github.com/JakkalaSathishKumar1234 |
| Lottery | Happy Saxena | https://github.com/HappySaxena |
| Lottery | Yesaswini Gorja | https://github.com/Yesaswini29 |

## Notes

- The scheduler expects valid integer input for all process attributes.
- In dummy mode, the file format must match the documented structure exactly.
