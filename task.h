#ifndef TASK_H
#define TASK_H

// Estrutura para representar uma tarefa (task)
typedef struct task {
    char *name;         // Nome da tarefa
    int priority;       // Prioridade da tarefa (menor número = maior prioridade)
    int burst;          // Tempo de CPU restante para a tarefa
    int originalBurst;  // Tempo de CPU original da tarefa (para cálculos de turnaround/wait time)
    int deadline;       // Prazo final da tarefa (usado no EDF)
    int arrivalTime;    // Tempo de chegada da tarefa no sistema
    int waitingTime;    // Tempo que a tarefa está esperando na fila (usado no Aging)
    int responseTime;   // Tempo de resposta (primeira vez que a tarefa começa a executar)
    int turnaroundTime; // Tempo de turnaround (tempo total desde a chegada até a conclusão)
    int completionTime; // Tempo de conclusão da tarefa
    int started;        // Flag para indicar se a tarefa já começou a executar
} Task;

#endif