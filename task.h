#ifndef TASK_H
#define TASK_H

// estrutura para uma tarefa
typedef struct task {
    char *name;         // Nome da tarefa
    int tid;            // ID da tarefa (Thread ID)
    int priority;       // Prioridade da tarefa (menor número = maior prioridade)
    int burst;          // Tempo de CPU restante necessário para a tarefa
    int deadline;       // Prazo final para a tarefa (para escalonadores EDF)
    int initial_burst;  // Tempo de CPU original da tarefa (para cálculos de turnaround/wait time)
    int arrival_time;   // Tempo em que a tarefa chegou ao sistema (para aging e métricas)
    int last_run_time;  // Último tempo global em que a tarefa foi executada (para aging)
} Task;

#endif