#ifndef SCHEDULE_RR_P_H
#define SCHEDULE_RR_P_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10 // A prioridade 1 é a mais alta

// Adiciona uma tarefa à lista de tarefas prontas com prioridade
void add_rr_p(char *name, int priority, int burst);

// Invoca o escalonador Round-Robin com Prioridade
void schedule_rr_p();

#endif
