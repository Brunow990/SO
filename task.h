#ifndef TASK_H
#define TASK_H

// estrutura para uma tarefa
typedef struct task {
    char *name;
    int tid;
    int priority;
    int burst;
    int deadline;       // Para o escalonador EDF
    int initial_burst;  // Para armazenar o tempo de burst original (para RR e envelhecimento)
    int arrival_time;   // Para rastrear o tempo de chegada (pode ser usado para envelhecimento)
    int last_run_time;  // Para rastrear quando a tarefa foi executada pela última vez (para envelhecimento)
} Task;

#endif