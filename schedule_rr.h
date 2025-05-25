#ifndef SCHEDULE_RR_H
#define SCHEDULE_RR_H

#define QUANTUM 10 // Define QUANTUM para RR especificamente se necessário, caso contrário usa o de CPU.h

// adiciona uma tarefa à lista para o escalonador Round Robin
void add_rr(char *name, int priority, int burst); // Prioridade pode ser ignorada no RR puro

// invoca o escalonador Round Robin
void schedule_rr();

#endif