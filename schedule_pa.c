#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedule_pa.h"
#include "task.h"
#include "list.h"
#include "CPU.h"

// Array de ponteiros de cabeçalho para filas de prioridade para Prioridade com Envelhecimento
#define PA_NUM_PRIORITY_LEVELS (MAX_PRIORITY - MIN_PRIORITY + 1)
struct node *pa_priority_queues[PA_NUM_PRIORITY_LEVELS];

static int pa_tid_counter = 0;
static int pa_global_time = 0;
#define AGING_THRESHOLD 50 // Defina seu limite de envelhecimento aqui (ex: 50 unidades de tempo)

void initialize_pa_priority_queues() {
    static int initialized = 0;
    if (!initialized) {
        for (int i = 0; i < PA_NUM_PRIORITY_LEVELS; i++) {
            pa_priority_queues[i] = NULL;
        }
        initialized = 1;
    }
}

// Função para aplicar envelhecimento
void apply_aging() {
    for (int i = 0; i < PA_NUM_PRIORITY_LEVELS - 1; i++) { // Não envelhece a prioridade mais alta (índice 0)
        struct node *current = pa_priority_queues[i + 1]; // Começa da próxima prioridade mais baixa
        struct node *prev = NULL;

        while (current != NULL) {
            Task *task = current->task;
            if (pa_global_time - task->last_run_time >= AGING_THRESHOLD && task->priority > MIN_PRIORITY) {
                printf("Envelhecimento: Tarefa %s (P%d) envelheceu para P%d\n", task->name, task->priority, task->priority - 1);
                task->priority--; // Aumenta a prioridade (diminui o número da prioridade)

                // Remove a tarefa da fila atual
                if (prev == NULL) { // Se for o cabeçalho
                    pa_priority_queues[i + 1] = current->next;
                } else {
                    prev->next = current->next;
                }
                struct node *node_to_move = current;
                current = current->next; // Move para a próxima tarefa na fila original

                // Insere na nova fila de prioridade mais alta
                node_to_move->next = NULL; // Desanexa da lista antiga
                insert(&pa_priority_queues[task->priority - MIN_PRIORITY], task);
                
                // A função `insert` adiciona ao cabeçalho. Para manter RR dentro de uma prioridade
                // após o envelhecimento, você pode querer inserir no final da nova fila.
                // Isso exigiria modificar `insert` ou ter um `insert_at_tail` separado.

            } else {
                prev = current;
                current = current->next;
            }
        }
    }
}


// adiciona uma tarefa à lista 
void add_pa(char *name, int priority, int burst){
    initialize_pa_priority_queues();

    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
        fprintf(stderr, "Prioridade inválida para a tarefa %s: %d (deve estar entre %d e %d)\n", name, priority, MIN_PRIORITY, MAX_PRIORITY);
        return;
    }

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
    newTask->tid = ++pa_tid_counter;
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->initial_burst = burst;
    newTask->arrival_time = pa_global_time;
    newTask->last_run_time = pa_global_time; // Inicializa o tempo da última execução

    // Insere na fila de prioridade correta (cabeçalho por enquanto)
    insert(&pa_priority_queues[priority - MIN_PRIORITY], newTask);
    printf("Tarefa adicionada [%s] prioridade [%d] burst [%d]\n", name, priority, burst);
}

// invoca o escalonador
void schedule_pa(){
    initialize_pa_priority_queues();
    
    int tasks_remaining = 1;
    while (tasks_remaining) {
        tasks_remaining = 0;
        
        apply_aging(); // Aplica o envelhecimento antes de escolher a próxima tarefa

        for (int i = 0; i < PA_NUM_PRIORITY_LEVELS; i++) {
            if (pa_priority_queues[i] != NULL) {
                tasks_remaining = 1;
                
                Task *current_task = pa_priority_queues[i]->task;

                // Remove a tarefa do cabeçalho (para reinserir no final se não estiver concluída)
                delete(&pa_priority_queues[i], current_task); 

                int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;
                
                run(current_task, slice);
                current_task->burst -= slice;
                pa_global_time += slice;

                current_task->last_run_time = pa_global_time; // Atualiza o tempo da última execução

                if (current_task->burst > 0) {
                    // Reinserir no final de sua fila de prioridade
                    struct node *temp = pa_priority_queues[i];
                    if (temp == NULL) {
                        insert(&pa_priority_queues[i], current_task);
                    } else {
                        while (temp->next != NULL) {
                            temp = temp->next;
                        }
                        struct node *newNode = malloc(sizeof(struct node));
                        if (newNode == NULL) {
                            perror("Falha ao alocar novo nó para reinserção");
                            exit(EXIT_FAILURE);
                        }
                        newNode->task = current_task;
                        newNode->next = NULL;
                        temp->next = newNode;
                    }
                } else {
                    printf("Tarefa %s finalizada.\n", current_task->name);
                }
                break; // Sai deste loop para reavaliar as prioridades após uma execução
            }
        }
    }
    printf("Escalonamento Prioridade com Envelhecimento completo. Tempo total: %d\n", pa_global_time);
}