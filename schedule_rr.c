#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedule_rr.h"
#include "task.h"
#include "list.h"
#include "CPU.h" // Para a função run e QUANTUM

// Cabeçalho da lista de tarefas Round Robin (RR puro usa uma única fila)
struct node *rr_task_list = NULL;
static int rr_tid_counter = 0; // Contador global para IDs de tarefas
static int rr_global_time = 0; // Tempo global para o escalonador RR

// adiciona uma tarefa à lista para o escalonador Round Robin
void add_rr(char *name, int priority, int burst){
    Task *newTask = (Task *) malloc(sizeof(Task));
    if (newTask == NULL) {
        perror("Falha ao alocar nova tarefa para RR");
        exit(EXIT_FAILURE);
    }
    newTask->name = strdup(name);
    if (newTask->name == NULL) {
        perror("Falha ao duplicar o nome da tarefa para RR");
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newTask->tid = ++rr_tid_counter;
    newTask->priority = priority; // Prioridade é ignorada no RR puro, mas mantida na struct
    newTask->burst = burst;
    newTask->initial_burst = burst; // Guarda o burst original para referência
    newTask->arrival_time = rr_global_time; // Define o tempo de chegada
    newTask->deadline = 0; // RR puro não usa deadline, pode ser 0 ou outro valor padrão

    // Para Round Robin, adiciona ao final da lista (garante a ordem RR)
    struct node *newNode = malloc(sizeof(struct node));
    if (newNode == NULL) {
        perror("Falha ao alocar novo nó para RR");
        free(newTask->name);
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newNode->task = newTask;
    newNode->next = NULL;

    if (rr_task_list == NULL) {
        rr_task_list = newNode;
    } else {
        struct node *temp = rr_task_list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    printf("Tarefa adicionada para RR [%s] burst [%d]\n", name, burst);
}

// invoca o escalonador Round Robin
void schedule_rr(){
    printf("Iniciando escalonamento Round Robin...\n");

    // Continua enquanto houver tarefas na lista
    while (rr_task_list != NULL) {
        Task *current_task = rr_task_list->task; // Pega a tarefa no cabeçalho da lista

        // Remove a tarefa do cabeçalho. Ela será reinserida no final se não for concluída.
        delete(&rr_task_list, current_task); 

        // Determina a fatia de tempo para execução (quantum ou burst restante)
        int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;
        
        // Executa a tarefa (simulada pela função run)
        run(current_task, slice); // A função run imprime a execução
        current_task->burst -= slice; // Diminui o burst restante da tarefa
        rr_global_time += slice; // Avança o tempo global do simulador

        // Se a tarefa ainda não foi concluída, reinserir no final da lista
        if (current_task->burst > 0) {
            // Re-insere a tarefa no final da lista.
            struct node *newNode = malloc(sizeof(struct node));
            if (newNode == NULL) {
                perror("Falha ao alocar novo nó para reinserção de tarefa RR");
                exit(EXIT_FAILURE);
            }
            newNode->task = current_task;
            newNode->next = NULL;

            if (rr_task_list == NULL) { // Se a lista ficou vazia por algum motivo inesperado
                rr_task_list = newNode;
            } else {
                struct node *temp = rr_task_list;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = newNode;
            }
        } else {
            // A tarefa foi concluída. A função `delete` já deve ter liberado a memória.
            printf("Tarefa %s finalizada.\n", current_task->name);
        }
    }
    printf("Escalonamento Round Robin completo. Tempo total: %d\n", rr_global_time);
}