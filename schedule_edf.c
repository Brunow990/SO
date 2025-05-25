#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h> // Para INT_MAX

#include "schedule_edf.h"
#include "task.h"
#include "list.h"
#include "CPU.h" // Para a função run

// Ponteiro para o cabeçalho da lista de tarefas para o escalonador EDF
// EDF geralmente usa uma única fila que é logicamente ordenada pelo deadline.
struct node *edf_task_list = NULL;
static int edf_tid_counter = 0; // Contador global para IDs de tarefas
static int edf_global_time = 0; // Tempo global para o escalonador EDF

// Função para encontrar a tarefa com o deadline mais cedo na lista
Task *pickNextTask_edf() {
    if (edf_task_list == NULL) {
        return NULL; // Nenhuma tarefa na lista
    }

    Task *earliest_deadline_task = NULL;
    struct node *current = edf_task_list;
    int min_deadline = INT_MAX; // Inicializa com o maior valor possível

    // Percorre a lista para encontrar a tarefa com o menor deadline
    while (current != NULL) {
        if (current->task->deadline < min_deadline) {
            min_deadline = current->task->deadline;
            earliest_deadline_task = current->task;
        }
        current = current->next;
    }
    return earliest_deadline_task;
}

// adiciona uma tarefa à lista para o escalonador EDF
void add_edf(char *name, int priority, int burst, int deadline){
    Task *newTask = (Task *) malloc(sizeof(Task));
    if (newTask == NULL) {
        perror("Falha ao alocar nova tarefa para EDF");
        exit(EXIT_FAILURE);
    }
    newTask->name = strdup(name);
    if (newTask->name == NULL) {
        perror("Falha ao duplicar o nome da tarefa para EDF");
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newTask->tid = ++edf_tid_counter;
    newTask->priority = priority; // Prioridade pode não ser usada diretamente no EDF puro
    newTask->burst = burst;
    newTask->initial_burst = burst; // Armazena o tempo de burst inicial
    newTask->deadline = deadline;
    newTask->arrival_time = edf_global_time; // Registra o tempo de chegada
    newTask->last_run_time = edf_global_time; // Inicializa o tempo da última execução

    insert(&edf_task_list, newTask); // Adiciona a tarefa à lista (a ordem será determinada por pickNextTask)
    printf("Tarefa adicionada para EDF [%s] prioridade [%d] burst [%d] deadline [%d]\n", name, priority, burst, deadline);
}

// invoca o escalonador EDF
void schedule_edf(){
    printf("Iniciando escalonamento Earliest Deadline First (EDF)...\n");

    while (edf_task_list != NULL) {
        Task *current_task = pickNextTask_edf(); // Seleciona a tarefa com o deadline mais cedo

        if (current_task == NULL) {
            break; // Nenhuma tarefa restante
        }

        // Executa a tarefa por um quantum ou seu burst restante, o que for menor
        int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;
        
        run(current_task, slice); // Simula a execução da tarefa
        current_task->burst -= slice; // Diminui o burst restante
        edf_global_time += slice; // Avança o tempo global

        current_task->last_run_time = edf_global_time; // Atualiza o tempo da última execução

        // Verifica se a tarefa foi concluída
        if (current_task->burst <= 0) {
            printf("Tarefa %s finalizada.\n", current_task->name);
            delete(&edf_task_list, current_task); // Remove da lista
        } else {
            // Se a tarefa não estiver concluída, ela permanece na lista.
            // A próxima iteração de pickNextTask_edf() irá reavaliar a ordem.
            // Verifica se a tarefa perdeu seu deadline
            if (edf_global_time > current_task->deadline) {
                printf("AVISO: A tarefa %s perdeu seu deadline! Tempo atual: %d, Deadline: %d\n", 
                       current_task->name, edf_global_time, current_task->deadline);
            }
        }
    }
    printf("Escalonamento EDF completo. Tempo total: %d\n", edf_global_time);
}