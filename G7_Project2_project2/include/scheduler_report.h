#ifndef SCHEDULER_REPORT_H
#define SCHEDULER_REPORT_H

#include "scheduler_types.h"

void printGantt(GanttEntry chart[], int chart_size);
void calculateMetrics(Process p[], int n);

#endif
