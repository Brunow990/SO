#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedule_rr.h"
#include "task.h"
#include "list.h"
#include "CPU.h" // Para a função run

// Cabeçalho da lista de tarefas Round Robin
struct node *rr_task_list = NULL;
static int rr_tid_counter = 0; // Contador global para IDs de tarefas
static int rr_global_time = 0; // Tempo global para RR

// adiciona uma tarefa à lista 
void add_rr(char *name, int priority, int burst){
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
    newTask->tid = ++rr_tid_counter;
    newTask->priority = priority; // Prioridade é ignorada no RR puro
    newTask->burst = burst;
    newTask->initial_burst = burst;
    newTask->arrival_time = rr_global_time;

    // Para Round Robin, adiciona ao final da lista
    struct node *newNode = malloc(sizeof(struct node));
    if (newNode == NULL) {
        perror("Falha ao alocar novo nó");
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
    printf("Tarefa adicionada [%s] burst [%d]\n", name, burst);
}

// invoca o escalonador
void schedule_rr(){
    while (rr_task_list != NULL) {
        Task *current_task = rr_task_list->task; // Pega a tarefa no cabeçalho

        // Remove do cabeçalho (para readicionar ao final se não estiver concluída)
        delete(&rr_task_list, current_task); 

        // Executa a tarefa por um quantum ou seu burst restante
        int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;
        
        run(current_task, slice);
        current_task->burst -= slice;
        rr_global_time += slice;

        // Se a tarefa não estiver concluída, reinserir no final da lista
        if (current_task->burst > 0) {
            // Reinserir no final (precisa percorrer até o final da lista)
            struct node *newNode = malloc(sizeof(struct node));
            if (newNode == NULL) {
                perror("Falha ao alocar novo nó para reinserção");
                exit(EXIT_FAILURE);
            }
            newNode->task = current_task;
            newNode->next = NULL;

            if (rr_task_list == NULL) { // Não deve acontecer se ainda houver tarefas
                rr_task_list = newNode;
            } else {
                struct node *temp = rr_task_list;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = newNode;
            }
        } else {
            printf("Tarefa %s finalizada.\n", current_task->name);
            // A memória da tarefa já foi liberada por delete() se implementado para isso
        }
    }
    printf("Escalonamento Round Robin completo. Tempo total: %d\n", rr_global_time);
}