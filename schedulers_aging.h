// schedulers_aging.h - Prioridade com Aging
#ifndef SCHEDULERS_AGING_H
#define SCHEDULERS_AGING_H

#include "task.h"

// insere nova tarefa
void add(Task *t);

// faz o escalonamento de acordo com Aging
void schedule();

#endif // SCHEDULERS_AGING_H