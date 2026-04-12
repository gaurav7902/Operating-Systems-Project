#ifndef SCHEDULER_ALGORITHMS_H
#define SCHEDULER_ALGORITHMS_H

#include "scheduler_types.h"

/* Team EDF implements this in src/algorithms/edf.c */
void runEDF(Process p[], int n, GanttEntry chart[], int *chart_size);

/* Team Lottery implements this in src/algorithms/lottery.c */
void runLottery(Process p[], int n, GanttEntry chart[], int *chart_size);

/* Team MLFQ implements this in src/algorithms/mlfq.c */
void runMLFQ(Process p[], int n, GanttEntry chart[], int *chart_size);

#endif
