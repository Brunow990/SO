#ifndef SCHEDULE_EDF_H
#define SCHEDULE_EDF_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// adiciona uma tarefa à lista 
void add_edf(char *name, int priority, int burst, int deadline);

// invoca o escalonador
void schedule_edf();

#endif