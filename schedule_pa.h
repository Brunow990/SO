#ifndef SCHEDULE_PA_H
#define SCHEDULE_PA_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// adiciona uma tarefa à lista para o escalonador de Prioridade com Envelhecimento
void add_pa(char *name, int priority, int burst);

// invoca o escalonador de Prioridade com Envelhecimento
void schedule_pa();

#endif