#ifndef SCHEDULE_PA_H
#define SCHEDULE_PA_H

#define MIN_PRIORITY_PA 1
#define MAX_PRIORITY_PA 10 // A prioridade 1 é a mais alta
#define AGING_THRESHOLD 50 // Limiar de tempo para envelhecimento (ex: 50 unidades de tempo)

// Adiciona uma tarefa à lista de tarefas prontas para Prioridade com Aging
void add_pa(char *name, int priority, int burst);

// Invoca o escalonador de Prioridade com Aging
void schedule_pa();

#endif
