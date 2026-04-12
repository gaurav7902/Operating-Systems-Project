# Project 2 Scheduler Skeleton

 Contributor workflow note:
 1. First understand your assigned task and expected behavior.
 2. Implement your part in the correct module.
 3. Before pushing your implementation, update this README with your part's documentation like what you added and any usage details for your part.
 4. Make a branch as `person1name-person2name-algorithm` then work

This folder is intentionally a skeleton so multiple contributors can work in parallel.

Current status:
- Shared data models are ready.
- Shared reporting functions are ready.
- Main program flow and algorithm selection are ready.
- Algorithm implementations are placeholders only.

## Current Structure

```
G7_Project2_project2/
├── Makefile
├── README.md
├── main.c
├── include/
│   ├── scheduler_types.h
│   ├── scheduler_report.h
│   └── scheduler_algorithms.h
└── src/
		├── report.c
		└── algorithms/
				├── edf.c
				├── lottery.c
				└── mlfq.c
```

## Team Responsibilities

1. Team EDF
- Implement `runEDF(...)` in `src/algorithms/edf.c`.

2. Team Lottery
- Implement `runLottery(...)` in `src/algorithms/lottery.c`.

3. Team MLFQ
- Implement `runMLFQ(...)` in `src/algorithms/mlfq.c`.

## Prototype Rules

- Keep all algorithm prototypes in `include/scheduler_algorithms.h`.
- If you add a new scheduling algorithm:
	- Add its prototype in `include/scheduler_algorithms.h`.
	- Add its source file in `src/algorithms/`.
	- Add selection logic in `main.c`.
	- Add the new source path to `Makefile` under `SOURCES`.

## Shared Files (Do Not Break)

- `include/scheduler_types.h`: shared `Process` and `GanttEntry` structs.
- `include/scheduler_report.h` and `src/report.c`: Gantt printing and metrics logic.
- `main.c`: input collection and algorithm dispatch.

## Build and Run

From this folder:

```bash
make
make run
make clean
```

## Notes for Contributors

- This is a collaboration skeleton, not a completed scheduler project.
- Use the existing function signatures so all files remain compatible.
- Keep changes scoped to your team file whenever possible to avoid merge conflicts.
