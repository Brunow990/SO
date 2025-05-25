#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedule_rr_p.h"
#include "task.h"
#include "list.h"
#include "CPU.h" // Para a função run e QUANTUM

// Array de ponteiros de cabeçalho para filas de prioridade (MÚLTIPLAS FILAS)
// O índice 0 corresponde à prioridade mais alta (MIN_PRIORITY, ex: 1)
// O índice (MAX_PRIORITY - MIN_PRIORITY) corresponde à prioridade mais baixa (MAX_PRIORITY, ex: 10)
#define RR_P_NUM_PRIORITY_LEVELS (MAX_PRIORITY - MIN_PRIORITY + 1)
struct node *rr_p_priority_queues[RR_P_NUM_PRIORITY_LEVELS];

static int rr_p_tid_counter = 0; // Contador global para IDs de tarefas
static int rr_p_global_time = 0; // Tempo global para o escalonador RR_P

// Inicializa as filas de prioridade (garante que todos os ponteiros sejam NULL no início)
void initialize_rr_p_priority_queues() {
    static int initialized = 0;
    if (!initialized) {
        for (int i = 0; i < RR_P_NUM_PRIORITY_LEVELS; i++) {
            rr_p_priority_queues[i] = NULL;
        }
        initialized = 1;
    }
}

// Função para escolher a próxima tarefa com base em Round Robin com Prioridade
// Prioridade é levada em conta primeiro, depois RR dentro da mesma prioridade.
Task *pickNextTask_rr_p() {
    initialize_rr_p_priority_queues(); // Garante que as filas estejam inicializadas

    // Itera das prioridades mais altas (índice 0) para as mais baixas
    for (int i = 0; i < RR_P_NUM_PRIORITY_LEVELS; i++) {
        if (rr_p_priority_queues[i] != NULL) {
            // Encontrou uma tarefa na fila de prioridade mais alta disponível.
            // Para Round Robin dentro da mesma prioridade, pega o cabeçalho.
            return rr_p_priority_queues[i]->task;
        }
    }
    return NULL; // Nenhuma tarefa em nenhuma fila
}

// adiciona uma tarefa à lista para o escalonador Round Robin com Prioridade
void add_rr_p(char *name, int priority, int burst){
    initialize_rr_p_priority_queues(); // Garante que as filas estejam inicializadas

    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
        fprintf(stderr, "Prioridade inválida para a tarefa %s: %d (deve estar entre %d e %d)\n", name, priority, MIN_PRIORITY, MAX_PRIORITY);
        return;
    }

    Task *newTask = (Task *) malloc(sizeof(Task));
    if (newTask == NULL) {
        perror("Falha ao alocar nova tarefa para RR_P");
        exit(EXIT_FAILURE);
    }
    newTask->name = strdup(name);
    if (newTask->name == NULL) {
        perror("Falha ao duplicar o nome da tarefa para RR_P");
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newTask->tid = ++rr_p_tid_counter;
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->initial_burst = burst; // Armazena o burst inicial
    newTask->arrival_time = rr_p_global_time; // Registra o tempo de chegada
    newTask->last_run_time = rr_p_global_time; // Inicializa o tempo da última execução
    newTask->deadline = 0; // RR_P não usa deadline, pode ser 0 ou outro valor padrão

    // Insere a tarefa na fila de prioridade correta (no final para manter RR dentro da prioridade)
    struct node *newNode = malloc(sizeof(struct node));
    if (newNode == NULL) {
        perror("Falha ao alocar novo nó para RR_P");
        free(newTask->name);
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newNode->task = newTask;
    newNode->next = NULL;

    struct node **target_queue_head = &rr_p_priority_queues[priority - MIN_PRIORITY];
    if (*target_queue_head == NULL) {
        *target_queue_head = newNode;
    } else {
        struct node *temp = *target_queue_head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    printf("Tarefa adicionada para RR_P [%s] prioridade [%d] burst [%d]\n", name, priority, burst);
}

// invoca o escalonador Round Robin com Prioridade
void schedule_rr_p(){
    initialize_rr_p_priority_queues(); // Garante que as filas estejam inicializadas
    printf("Iniciando escalonamento Round Robin com Prioridade (RR_p)...\n");
    
    int tasks_remaining = 1; // Assume que há tarefas restantes até que se prove o contrário
    while (tasks_remaining) {
        tasks_remaining = 0; // Reseta para esta iteração

        // Itera das prioridades mais altas (índice 0) para as mais baixas
        for (int i = 0; i < RR_P_NUM_PRIORITY_LEVELS; i++) {
            // Verifica se esta fila de prioridade tem tarefas
            if (rr_p_priority_queues[i] != NULL) {
                tasks_remaining = 1; // Temos tarefas, continua escalonando
                
                // Pega a tarefa no cabeçalho da fila de prioridade atual
                Task *current_task = rr_p_priority_queues[i]->task;

                // Remove a tarefa do cabeçalho da fila (para reinserir no final se não estiver concluída)
                delete(&rr_p_priority_queues[i], current_task); 

                // Executa a tarefa por um quantum ou seu burst restante, o que for menor
                int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;
                
                run(current_task, slice); // Simula a execução da tarefa
                current_task->burst -= slice; // Diminui o burst restante
                rr_p_global_time += slice; // Avança o tempo global

                current_task->last_run_time = rr_p_global_time; // Atualiza o tempo da última execução

                // Se a tarefa não estiver concluída, reinserir no final de sua fila de prioridade
                if (current_task->burst > 0) {
                    // Re-insere a tarefa no final da fila de prioridade atual
                    struct node *newNode = malloc(sizeof(struct node));
                    if (newNode == NULL) {
                        perror("Falha ao alocar novo nó para reinserção de tarefa RR_P");
                        exit(EXIT_FAILURE);
                    }
                    newNode->task = current_task;
                    newNode->next = NULL;

                    struct node **target_queue_head = &rr_p_priority_queues[current_task->priority - MIN_PRIORITY];
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
    printf("Escalonamento Round Robin com Prioridade completo. Tempo total: %d\n", rr_p_global_time);
}