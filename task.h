// task.h  (use este em todos os módulos)
#ifndef TASK_H
#define TASK_H

typedef struct Task {
    char *name;
    int   tid;
    int   priority;      // 1 = maior prioridade
    int   burst;         // tempo restante de execução
    int   deadline;      // usado no EDF
    int   waiting_time;  // usado no Aging
} Task;

#endif // TASK_H