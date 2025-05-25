#ifndef SCHEDULE_PA_H
#define SCHEDULE_PA_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// adiciona uma tarefa à lista 
void add_pa(char *name, int priority, int burst);

// invoca o escalonador
void schedule_pa();

#endif