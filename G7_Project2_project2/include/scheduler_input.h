#ifndef SCHEDULER_INPUT_H
#define SCHEDULER_INPUT_H

#include "scheduler_types.h"

int chooseInputMode(void);
void readProcesses(Process p[], int n);
int loadDummyProcesses(Process p[], int *n);
void printProcessData(const Process p[], int n);
int chooseAlgorithm(void);

#endif
