#ifndef SCHEDULE_EDF_H
#define SCHEDULE_EDF_H

// Adiciona uma tarefa à lista de tarefas prontas para EDF
void add_edf(char *name, int priority, int burst, int deadline);

// Invoca o escalonador EDF
void schedule_edf();

#endif
