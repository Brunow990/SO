#ifndef CPU_H
#define CPU_H

// Duração de um quantum de tempo
#define QUANTUM 10

#include "task.h"

// Executa a tarefa especificada pelo tempo de fatia
void run(Task *task, int slice);

#endif
