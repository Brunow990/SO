#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h> // Para INT_MAX

#include "schedule_edf.h"
#include "task.h"
#include "list.h"
#include "CPU.h" // Para a função run

// Ponteiro para o cabeçalho da lista de tarefas
struct node *edf_task_list = NULL;
static int tid_counter = 0; // Contador global para IDs de tarefas

// Simulação de temporizador
static int global_time = 0;
// Sinalizador para expiração do quantum (simulado pelo loop do escalonador por enquanto)
// Em um cenário real, isso seria definido por uma thread de temporizador.
static volatile int quantum_expired = 0; 

// Função para encontrar a tarefa com o deadline mais cedo
Task *pickNextTask_edf() {
    if (edf_task_list == NULL) {
        return NULL;
    }

    Task *earliest_deadline_task = NULL;
    struct node *current = edf_task_list;
    int min_deadline = INT_MAX;

    while (current != NULL) {
        if (current->task->deadline < min_deadline) {
            min_deadline = current->task->deadline;
            earliest_deadline_task = current->task;
        }
        current = current->next;
    }
    return earliest_deadline_task;
}


// adiciona uma tarefa à lista 
void add_edf(char *name, int priority, int burst, int deadline){
    Task *newTask = (Task *) malloc(sizeof(Task));
    if (newTask == NULL) {
        perror("Falha ao alocar nova tarefa");
        exit(EXIT_FAILURE);
    }
    newTask->name = strdup(name);
    if (newTask->name == NULL) {
        perror("Falha ao duplicar o nome da tarefa");
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newTask->tid = ++tid_counter;
    newTask->priority = priority; // A prioridade pode não ser usada diretamente no EDF puro, mas mantida por compatibilidade
    newTask->burst = burst;
    newTask->initial_burst = burst; // Armazena o tempo de burst inicial
    newTask->deadline = deadline;
    newTask->arrival_time = global_time; // Registra o tempo de chegada

    insert(&edf_task_list, newTask);
    printf("Tarefa adicionada [%s] prioridade [%d] burst [%d] deadline [%d]\n", name, priority, burst, deadline);
}

// invoca o escalonador
void schedule_edf(){
    while (edf_task_list != NULL) {
        Task *current_task = pickNextTask_edf();

        if (current_task == NULL) {
            break; // Nenhuma tarefa restante
        }

        // Executa a tarefa por um quantum ou seu burst restante, o que for menor
        int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;
        
        run(current_task, slice); // Simula a execução da tarefa
        current_task->burst -= slice; // Diminui o burst restante
        global_time += slice; // Avança o tempo global

        current_task->last_run_time = global_time; // Atualiza o tempo da última execução

        // Verifica se a tarefa foi concluída
        if (current_task->burst <= 0) {
            printf("Tarefa %s finalizada.\n", current_task->name);
            delete(&edf_task_list, current_task); // Remove da lista
        } else {
            // Se a tarefa não estiver concluída, ela permanece na lista.
            // Para EDF, a lista é reavaliada para a próxima tarefa com o deadline mais cedo.
            // Se a tarefa perdeu seu deadline, você pode querer imprimir uma mensagem ou lidar com isso.
            if (global_time > current_task->deadline) {
                printf("AVISO: A tarefa %s perdeu seu deadline! Tempo atual: %d, Deadline: %d\n", 
                       current_task->name, global_time, current_task->deadline);
            }
        }
    }
    printf("Escalonamento EDF completo. Tempo total: %d\n", global_time);
}