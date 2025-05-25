#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> // Para INT_MAX
#include <stdbool.h>

#include "schedule_edf.h"
#include "list.h"
#include "CPU.h"

// Lista de tarefas prontas para o escalonador EDF
struct node *edf_task_list = NULL;

// Variável global para o tempo atual do sistema
int current_time_edf = 0;

// Adiciona uma tarefa à lista de tarefas prontas para EDF
void add_edf(char *name, int priority, int burst, int deadline) {
    // Aloca memória para a nova tarefa
    Task *newTask = malloc(sizeof(Task));
    if (newTask == NULL) {
        perror("Falha ao alocar memória para a nova tarefa");
        exit(EXIT_FAILURE);
    }

    newTask->name = strdup(name); // Copia o nome da tarefa
    if (newTask->name == NULL) {
        perror("Falha ao alocar memória para o nome da tarefa");
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newTask->priority = priority; // Prioridade não é usada diretamente no EDF puro
    newTask->burst = burst;
    newTask->originalBurst = burst; // Salva o burst original
    newTask->deadline = deadline;
    newTask->arrivalTime = current_time_edf; // Define o tempo de chegada
    newTask->waitingTime = 0;       // Inicializa o tempo de espera
    newTask->responseTime = -1;     // Inicializa o tempo de resposta como -1 (não iniciado)
    newTask->turnaroundTime = 0;
    newTask->completionTime = 0;
    newTask->started = 0;           // Tarefa ainda não iniciou

    // Insere a tarefa na lista
    insert(&edf_task_list, newTask);
}

// Invoca o escalonador EDF
void schedule_edf() {
    printf("Iniciando escalonamento Earliest Deadline First (EDF)...\n");

    while (edf_task_list != NULL) {
        Task *next_task = NULL;
        int min_deadline = INT_MAX; // Inicializa com o maior valor possível

        // Encontra a tarefa com o menor deadline entre as tarefas prontas
        struct node *temp = edf_task_list;
        while (temp != NULL) {
            if (temp->task->deadline < min_deadline) {
                min_deadline = temp->task->deadline;
                next_task = temp->task;
            }
            temp = temp->next;
        }

        if (next_task == NULL) {
            break; // Nenhuma tarefa encontrada, embora a lista não seja nula (erro lógico)
        }

        // Atualiza o tempo de resposta se a tarefa estiver começando pela primeira vez
        if (next_task->started == 0) {
            next_task->responseTime = current_time_edf;
            next_task->started = 1;
        }

        // Calcula o tempo de execução para este quantum
        int slice = (next_task->burst < QUANTUM) ? next_task->burst : QUANTUM;

        // Executa a tarefa
        run(next_task, slice);
        next_task->burst -= slice;
        current_time_edf += slice; // Avança o tempo do sistema

        // Atualiza o tempo de espera para todas as outras tarefas na fila
        temp = edf_task_list;
        while (temp != NULL) {
            if (temp->task != next_task) {
                temp->task->waitingTime += slice;
            }
            temp = temp->next;
        }

        // Verifica se a tarefa foi concluída
        if (next_task->burst <= 0) {
            next_task->completionTime = current_time_edf;
            next_task->turnaroundTime = next_task->completionTime - next_task->arrivalTime;
            printf("Tarefa [%s] concluída no tempo %d. Turnaround: %d, Resposta: %d, Espera: %d\n",
                   next_task->name, next_task->completionTime, next_task->turnaroundTime,
                   next_task->responseTime, next_task->waitingTime);
            delete_task_by_name(&edf_task_list, next_task->name); // Remove a tarefa da lista
        }

        // Verifica se alguma tarefa perdeu seu deadline
        temp = edf_task_list;
        while (temp != NULL) {
            if (current_time_edf > temp->task->deadline && temp->task->burst > 0) {
                printf("AVISO: Tarefa [%s] perdeu seu deadline! Tempo atual: %d, Deadline: %d\n",
                       temp->task->name, current_time_edf, temp->task->deadline);
            }
            temp = temp->next;
        }
    }

    printf("Escalonamento EDF concluído. Tempo total: %d\n", current_time_edf);
}
