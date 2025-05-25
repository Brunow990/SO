#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedule_pa.h"
#include "task.h"
#include "list.h"
#include "CPU.h"

// Array de ponteiros de cabeçalho para filas de prioridade (MÚLTIPLAS FILAS)
// O índice 0 corresponde à prioridade mais alta (MIN_PRIORITY, ex: 1)
// O índice (MAX_PRIORITY - MIN_PRIORITY) corresponde à prioridade mais baixa (MAX_PRIORITY, ex: 10)
#define PA_NUM_PRIORITY_LEVELS (MAX_PRIORITY - MIN_PRIORITY + 1)
struct node *pa_priority_queues[PA_NUM_PRIORITY_LEVELS];

static int pa_tid_counter = 0;
static int pa_global_time = 0;
#define AGING_THRESHOLD 50 // Defina seu limite de envelhecimento aqui (ex: 50 unidades de tempo sem executar)

// Inicializa as filas de prioridade (garante que todos os ponteiros sejam NULL no início)
void initialize_pa_priority_queues() {
    static int initialized = 0;
    if (!initialized) {
        for (int i = 0; i < PA_NUM_PRIORITY_LEVELS; i++) {
            pa_priority_queues[i] = NULL;
        }
        initialized = 1;
    }
}

// Função para aplicar o envelhecimento (aging) às tarefas
void apply_aging() {
    // Itera das prioridades mais baixas para as mais altas (exceto a mais alta, que não pode envelhecer mais)
    for (int i = PA_NUM_PRIORITY_LEVELS - 1; i > 0; i--) { 
        struct node *current = pa_priority_queues[i];
        struct node *prev = NULL;

        while (current != NULL) {
            Task *task = current->task;
            // Verifica se a tarefa passou do limite de envelhecimento e não é a prioridade mais alta
            if (pa_global_time - task->last_run_time >= AGING_THRESHOLD && task->priority > MIN_PRIORITY) {
                printf("Envelhecimento: Tarefa %s (P%d) envelheceu para P%d\n", task->name, task->priority, task->priority - 1);
                task->priority--; // Aumenta a prioridade (diminui o número da prioridade)

                // Remove a tarefa da fila atual
                if (prev == NULL) { // Se for o cabeçalho da fila
                    pa_priority_queues[i] = current->next;
                } else {
                    prev->next = current->next;
                }
                struct node *node_to_move = current;
                current = current->next; // Move para a próxima tarefa na fila original

                // Insere a tarefa na nova fila de prioridade mais alta (no final para manter RR)
                node_to_move->next = NULL; // Desanexa do nó anterior
                
                // Encontra o final da nova fila de prioridade para inserção
                struct node **new_head = &pa_priority_queues[task->priority - MIN_PRIORITY];
                struct node *temp_new_queue = *new_head;
                if (temp_new_queue == NULL) {
                    *new_head = node_to_move; // Se a nova fila estiver vazia, torna-se o cabeçalho
                } else {
                    while (temp_new_queue->next != NULL) {
                        temp_new_queue = temp_new_queue->next;
                    }
                    temp_new_queue->next = node_to_move; // Adiciona ao final da nova fila
                }
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
}

// adiciona uma tarefa à lista para o escalonador de Prioridade com Envelhecimento
void add_pa(char *name, int priority, int burst){
    initialize_pa_priority_queues(); // Garante que as filas estejam inicializadas

    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
        fprintf(stderr, "Prioridade inválida para a tarefa %s: %d (deve estar entre %d e %d)\n", name, priority, MIN_PRIORITY, MAX_PRIORITY);
        return;
    }

    Task *newTask = (Task *) malloc(sizeof(Task));
    if (newTask == NULL) {
        perror("Falha ao alocar nova tarefa para PA");
        exit(EXIT_FAILURE);
    }
    newTask->name = strdup(name);
    if (newTask->name == NULL) {
        perror("Falha ao duplicar o nome da tarefa para PA");
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newTask->tid = ++pa_tid_counter;
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->initial_burst = burst;
    newTask->arrival_time = pa_global_time;
    newTask->last_run_time = pa_global_time; // Inicializa o tempo da última execução

    // Insere a tarefa na fila de prioridade correta (no final para manter RR dentro da prioridade)
    struct node *newNode = malloc(sizeof(struct node));
    if (newNode == NULL) {
        perror("Falha ao alocar novo nó para PA");
        free(newTask->name);
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newNode->task = newTask;
    newNode->next = NULL;

    struct node **target_queue_head = &pa_priority_queues[priority - MIN_PRIORITY];
    if (*target_queue_head == NULL) {
        *target_queue_head = newNode;
    } else {
        struct node *temp = *target_queue_head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    printf("Tarefa adicionada para PA [%s] prioridade [%d] burst [%d]\n", name, priority, burst);
}

// invoca o escalonador de Prioridade com Envelhecimento
void schedule_pa(){
    initialize_pa_priority_queues(); // Garante que as filas estejam inicializadas
    printf("Iniciando escalonamento Prioridade com Envelhecimento (PA)...\n");
    
    int tasks_remaining = 1; // Assume que há tarefas restantes até que se prove o contrário
    while (tasks_remaining) {
        tasks_remaining = 0; // Reseta para esta iteração

        apply_aging(); // Aplica o envelhecimento antes de escolher a próxima tarefa

        // Itera das prioridades mais altas para as mais baixas
        for (int i = 0; i < PA_NUM_PRIORITY_LEVELS; i++) {
            // Verifica se esta fila de prioridade tem tarefas
            if (pa_priority_queues[i] != NULL) {
                tasks_remaining = 1; // Temos tarefas, continua escalonando
                
                // Pega a tarefa no cabeçalho da fila de prioridade atual
                Task *current_task = pa_priority_queues[i]->task;

                // Remove a tarefa do cabeçalho da fila (para reinserir no final se não estiver concluída)
                delete(&pa_priority_queues[i], current_task); 

                // Executa a tarefa por um quantum ou seu burst restante, o que for menor
                int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;
                
                run(current_task, slice); // Simula a execução da tarefa
                current_task->burst -= slice; // Diminui o burst restante
                pa_global_time += slice; // Avança o tempo global

                current_task->last_run_time = pa_global_time; // Atualiza o tempo da última execução

                // Se a tarefa não estiver concluída, reinserir no final de sua fila de prioridade
                if (current_task->burst > 0) {
                    // Re-insere a tarefa no final da fila de prioridade atual
                    struct node *newNode = malloc(sizeof(struct node));
                    if (newNode == NULL) {
                        perror("Falha ao alocar novo nó para reinserção de tarefa PA");
                        exit(EXIT_FAILURE);
                    }
                    newNode->task = current_task;
                    newNode->next = NULL;

                    struct node **target_queue_head = &pa_priority_queues[current_task->priority - MIN_PRIORITY];
                    if (*target_queue_head == NULL) {
                        *target_queue_head = newNode;
                    } else {
                        struct node *temp = *target_queue_head;
                        while (temp->next != NULL) {
                            temp = temp->next;
                        }
                        temp->next = newNode;
                    }
                } else {
                    printf("Tarefa %s finalizada.\n", current_task->name);
                    // A memória da tarefa já foi liberada pela função 'delete'.
                }
                break; // Sai deste loop para reavaliar as prioridades após uma execução
            }
        }
    }
    printf("Escalonamento Prioridade com Envelhecimento completo. Tempo total: %d\n", pa_global_time);
}