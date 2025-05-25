#ifndef SCHEDULE_EDF_H
#define SCHEDULE_EDF_H
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// add a task to the list 
void add(char *name, int priority, int burst, int deadline);

// invoke the scheduler
void schedule();

#endif