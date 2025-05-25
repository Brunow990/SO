#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "schedule_rr_p.h"
#include "list.h"
#include "CPU.h"

// Array de listas de tarefas prontas, uma para cada nível de prioridade
// prioridade 1 é a mais alta, então o índice 0 será para prioridade 1
struct node *priority_queues[MAX_PRIORITY];

// Variável global para o tempo atual do sistema
int current_time_rr_p = 0;

// Inicializa as filas de prioridade
void init_priority_queues() {
    for (int i = 0; i < MAX_PRIORITY; i++) {
        priority_queues[i] = NULL;
    }
}

// Adiciona uma tarefa à lista de tarefas prontas com prioridade
void add_rr_p(char *name, int priority, int burst) {
    // Verifica se a prioridade está dentro dos limites válidos
    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
        fprintf(stderr, "Erro: Prioridade %d fora do intervalo válido (%d-%d) para a tarefa %s\n",
                priority, MIN_PRIORITY, MAX_PRIORITY, name);
        return;
    }

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
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->originalBurst = burst; // Salva o burst original
    newTask->deadline = 0;          // Não usado para RR_p
    newTask->arrivalTime = current_time_rr_p; // Define o tempo de chegada
    newTask->waitingTime = 0;       // Inicializa o tempo de espera
    newTask->responseTime = -1;     // Inicializa o tempo de resposta como -1 (não iniciado)
    newTask->turnaroundTime = 0;
    newTask->completionTime = 0;
    newTask->started = 0;           // Tarefa ainda não iniciou

    // Insere a tarefa na fila de prioridade correspondente
    // A prioridade 1 vai para priority_queues[0], prioridade 2 para priority_queues[1], etc.
    insert(&priority_queues[priority - 1], newTask);
}

// Invoca o escalonador Round-Robin com Prioridade
void schedule_rr_p() {
    printf("Iniciando escalonamento Round Robin com Prioridade...\n");

    init_priority_queues(); // Garante que as filas estão inicializadas

    bool tasks_remaining = true;
    while (tasks_remaining) {
        tasks_remaining = false; // Assume que não há mais tarefas, a menos que uma seja encontrada

        // Percorre as filas de prioridade, da mais alta (1) para a mais baixa (MAX_PRIORITY)
        for (int i = 0; i < MAX_PRIORITY; i++) {
            struct node *current_queue_head = priority_queues[i];

            // Se a fila de prioridade atual não estiver vazia
            if (current_queue_head != NULL) {
                tasks_remaining = true; // Há tarefas restantes

                struct node *current_node = current_queue_head;
                struct node *prev_node = NULL;
                Task *current_task = NULL;

                // Encontra a próxima tarefa na fila (FIFO) dentro desta prioridade
                // Como 'insert' adiciona no início, a tarefa mais antiga está no final da lista.
                // Precisamos percorrer a lista para encontrar a tarefa mais antiga.
                if (current_node->next == NULL) {
                    current_task = current_node->task;
                    priority_queues[i] = NULL; // A fila fica vazia temporariamente
                } else {
                    // Percorre para encontrar o último nó (o mais antigo na fila FIFO)
                    while (current_node->next != NULL) {
                        prev_node = current_node;
                        current_node = current_node->next;
                    }
                    current_task = current_node->task;
                    // Remove o último nó da lista
                    if (prev_node != NULL) {
                        prev_node->next = NULL;
                    }
                }

                if (current_task == NULL) {
                    continue; // Pula para a próxima fila se não houver tarefa válida
                }

                // Atualiza o tempo de resposta se a tarefa estiver começando pela primeira vez
                if (current_task->started == 0) {
                    current_task->responseTime = current_time_rr_p;
                    current_task->started = 1;
                }

                // Calcula o tempo de execução para este quantum
                int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;

                // Executa a tarefa
                run(current_task, slice);
                current_task->burst -= slice;
                current_time_rr_p += slice; // Avança o tempo do sistema

                // Atualiza o tempo de espera para todas as outras tarefas em TODAS as filas
                for (int j = 0; j < MAX_PRIORITY; j++) {
                    struct node *temp = priority_queues[j];
                    while (temp != NULL) {
                        if (temp->task != current_task) {
                            temp->task->waitingTime += slice;
                        }
                        temp = temp->next;
                    }
                }

                // Se a tarefa não terminou, insere-a de volta no final da fila de sua prioridade
                if (current_task->burst > 0) {
                    insert(&priority_queues[i], current_task);
                } else {
                    // Tarefa concluída
                    current_task->completionTime = current_time_rr_p;
                    current_task->turnaroundTime = current_task->completionTime - current_task->arrivalTime;
                    printf("Tarefa [%s] concluída no tempo %d. Turnaround: %d, Resposta: %d, Espera: %d\n",
                           current_task->name, current_task->completionTime, current_task->turnaroundTime,
                           current_task->responseTime, current_task->waitingTime);
                    // Libera a memória da tarefa e do nó
                    free(current_task->name);
                    free(current_task);
                    free(current_node); // Libera o nó que foi removido da lista
                }
                // Uma tarefa foi executada, então reinicia o loop para verificar as prioridades novamente
                // Isso garante que uma tarefa de maior prioridade que chegou seja executada imediatamente.
                i = -1; // Reinicia o loop 'for' para a prioridade mais alta
                break;  // Sai do loop interno e força a reavaliação das prioridades
            }
        }
    }

    printf("Escalonamento Round Robin com Prioridade concluído. Tempo total: %d\n", current_time_rr_p);
}
