#ifndef SCHEDULE_RR_P_H
#define SCHEDULE_RR_P_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// adiciona uma tarefa à lista 
void add_rr_p(char *name, int priority, int burst);

// invoca o escalonador
void schedule_rr_p();

#endif