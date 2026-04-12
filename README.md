# Operating Systems Project Portfolio

1. [Project 1: xv6 Custom System Calls and Kernel Extensions](G7_Project1_xv6CustomizeSystemCalls/README.md)
2. [Project 2: Process Scheduling Simulator](G7_Project2_project2/README.md)

Project 1 extends xv6-riscv with custom system calls, kernel-side synchronization, process inspection, IPC, signal handling, and priority-based scheduling. Project 2 is a C-based scheduler simulator that evaluates EDF, Lottery Scheduling, and MLFQ on the same process model.

## Repository Layout

```text
Operating-Systems-Project/
├── README.md
├── G7_Project1_xv6CustomizeSystemCalls/
│   ├── README.md
│   ├── screenshots/
│   └── xv6/
└── G7_Project2_project2/
	├── README.md
	├── example_processes_data.txt
	├── main.c
	├── Makefile
	├── include/
	└── src/
```

## Project 1 Overview

[Project 1 README](G7_Project1_xv6CustomizeSystemCalls/README.md) documents a set of xv6 kernel and user-space extensions:

- Alarm signal delivery with `alarm_signal()` and `alarm_return()`.
- Mailbox-based message passing with `sendmsg()` and `recvmsg()`.
- Process information queries with `getppid()` and `getprocinfo()`.
- Buffered syscall logging that prints per-process call history after exit.
- Counting semaphores with `sem_init()`, `sem_wait()`, and `sem_signal()`.
- Execution control with `yield_cpu()`, `sleep_for()`, and `fork_with_limit()`.
- Priority-based scheduling with `set_priority()` and a scheduler that selects the highest-priority runnable process.

The implementation touches the process table, trap handling, syscall dispatch, synchronization primitives, and user programs. It demonstrates how xv6 can be extended with signal-like behavior, IPC, process introspection, and scheduler control without changing the core teaching structure.

The delivered user programs include `alarm_test`, `mailboxtest`, `getprocinfo`, `syscall_test`, `semtest`, `testyield`, `testsleep`, `testforklimit`, and `testpriority`.

## Project 2 Overview

[Project 2 README](G7_Project2_project2/README.md) describes a scheduler simulator that implements:

- Earliest Deadline First (EDF)
- Lottery Scheduling
- Multi-Level Feedback Queue (MLFQ)

The simulator accepts either manual input or sample data from [example_processes_data.txt](G7_Project2_project2/example_processes_data.txt), then runs the selected algorithm, prints a Gantt chart, and reports completion, turnaround, waiting, and response times. The shared data model also tracks arrival time, burst time, I/O behavior, priority, deadline, tickets, and response state so each algorithm can be compared on the same workload.

## Contributors

### Project 1 and Project 2 Contributors

- John Gunji - https://github.com/johngunji
- Gaurav Patidar - https://github.com/gaurav7902
- Ishika Acharya - https://github.com/mockingjay777
- Sathish Kumar Jakkala - https://github.com/JakkalaSathishKumar1234
- Happy Saxena - https://github.com/HappySaxena
- Yesaswini Gorja - https://github.com/Yesaswini29

The two projects share the same contributor group, but each README describes the parts they implemented in that specific assignment.

## Build And Run

### Project 1

```bash
cd G7_Project1_xv6CustomizeSystemCalls/xv6
make clean
make CPUS=1 qemu
```

Inside the xv6 shell, run commands such as:

```bash
alarm_test
mailboxtest
getprocinfo
syscall_test
semtest
testyield
testsleep 50
testforklimit 2
testpriority 5 10 20
```

### Project 2

```bash
cd G7_Project2_project2
make
make run
make clean
```

Use the prompts to choose manual input or dummy data, then pick EDF, Lottery, or MLFQ.

## References

- [Project 1 detailed documentation](G7_Project1_xv6CustomizeSystemCalls/README.md)
- [Project 2 detailed documentation](G7_Project2_project2/README.md)
- xv6-riscv: https://github.com/mit-pdos/xv6-riscv
- xv6 course materials: https://pdos.csail.mit.edu/6.S081/2021/xv6/
