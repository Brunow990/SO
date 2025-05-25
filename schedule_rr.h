#ifndef SCHEDULE_RR_H
#define SCHEDULE_RR_H

#include "task.h"

// Adiciona uma tarefa à lista de tarefas prontas
void add_rr(char *name, int priority, int burst);

// Invoca o escalonador Round-Robin
void schedule_rr();

#endif
