#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "schedule_pa.h"
#include "list.h"
#include "CPU.h"

// Array de listas de tarefas prontas, uma para cada nível de prioridade
struct node *pa_priority_queues[MAX_PRIORITY_PA];

// Variável global para o tempo atual do sistema
int current_time_pa = 0;

// Inicializa as filas de prioridade para Prioridade com Aging
void init_pa_priority_queues() {
    for (int i = 0; i < MAX_PRIORITY_PA; i++) {
        pa_priority_queues[i] = NULL;
    }
}

// Adiciona uma tarefa à lista de tarefas prontas para Prioridade com Aging
void add_pa(char *name, int priority, int burst) {
    if (priority < MIN_PRIORITY_PA || priority > MAX_PRIORITY_PA) {
        fprintf(stderr, "Erro: Prioridade %d fora do intervalo válido (%d-%d) para a tarefa %s\n",
                priority, MIN_PRIORITY_PA, MAX_PRIORITY_PA, name);
        return;
    }

    Task *newTask = malloc(sizeof(Task));
    if (newTask == NULL) {
        perror("Falha ao alocar memória para a nova tarefa");
        exit(EXIT_FAILURE);
    }

    newTask->name = strdup(name);
    if (newTask->name == NULL) {
        perror("Falha ao alocar memória para o nome da tarefa");
        free(newTask);
        exit(EXIT_FAILURE);
    }
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->originalBurst = burst;
    newTask->deadline = 0; // Não usado para PA
    newTask->arrivalTime = current_time_pa;
    newTask->waitingTime = 0;
    newTask->responseTime = -1;
    newTask->turnaroundTime = 0;
    newTask->completionTime = 0;
    newTask->started = 0;

    // Insere a tarefa na fila de prioridade correspondente
    insert(&pa_priority_queues[priority - 1], newTask);
}

// Invoca o escalonador de Prioridade com Aging
void schedule_pa() {
    printf("Iniciando escalonamento de Prioridade com Aging...\n");

    init_pa_priority_queues(); // Garante que as filas estão inicializadas

    bool tasks_remaining = true;
    while (tasks_remaining) {
        tasks_remaining = false; // Assume que não há mais tarefas, a menos que uma seja encontrada

        Task *next_task = NULL;
        struct node *task_node_to_remove = NULL;
        int current_priority_level = -1; // Para saber de qual fila a tarefa foi pega

        // Percorre as filas de prioridade, da mais alta (1) para a mais baixa (MAX_PRIORITY_PA)
        for (int i = 0; i < MAX_PRIORITY_PA; i++) {
            if (pa_priority_queues[i] != NULL) {
                tasks_remaining = true; // Há tarefas restantes

                // Encontra a tarefa mais antiga na fila de maior prioridade não vazia
                struct node *current_node = pa_priority_queues[i];
                struct node *prev_node = NULL;

                if (current_node->next == NULL) {
                    next_task = current_node->task;
                    task_node_to_remove = current_node;
                    pa_priority_queues[i] = NULL; // A fila fica vazia temporariamente
                } else {
                    while (current_node->next != NULL) {
                        prev_node = current_node;
                        current_node = current_node->next;
                    }
                    next_task = current_node->task;
                    task_node_to_remove = current_node;
                    if (prev_node != NULL) {
                        prev_node->next = NULL;
                    }
                }
                current_priority_level = i;
                break; // Encontrou a tarefa de maior prioridade, sai do loop de prioridades
            }
        }

        if (next_task == NULL) {
            // Se não há tarefas em nenhuma fila, sai do loop principal
            if (!tasks_remaining) {
                break;
            }
            // Se tasks_remaining é true mas next_task é NULL, significa que a lógica de seleção falhou,
            // ou todas as tarefas em filas não vazias foram temporariamente removidas.
            // Isso pode acontecer se a lista for esvaziada e re-preenchida no mesmo ciclo.
            // Para evitar loop infinito, vamos avançar o tempo e tentar novamente.
            current_time_pa += 1; // Avança o tempo em 1 unidade para tentar o aging
            printf("Nenhuma tarefa selecionada, avançando tempo. Tempo atual: %d\n", current_time_pa);
            // Atualiza o tempo de espera para todas as tarefas
            for (int j = 0; j < MAX_PRIORITY_PA; j++) {
                struct node *temp_queue = pa_priority_queues[j];
                while (temp_queue != NULL) {
                    temp_queue->task->waitingTime += 1; // Incrementa o tempo de espera
                    temp_queue = temp_queue->next;
                }
            }
            continue; // Reinicia o loop while
        }

        // Atualiza o tempo de resposta se a tarefa estiver começando pela primeira vez
        if (next_task->started == 0) {
            next_task->responseTime = current_time_pa;
            next_task->started = 1;
        }

        // Calcula o tempo de execução para este quantum
        int slice = (next_task->burst < QUANTUM) ? next_task->burst : QUANTUM;

        // Executa a tarefa
        run(next_task, slice);
        next_task->burst -= slice;
        current_time_pa += slice; // Avança o tempo do sistema

        // Atualiza o tempo de espera para todas as outras tarefas em TODAS as filas
        for (int j = 0; j < MAX_PRIORITY_PA; j++) {
            struct node *temp_queue = pa_priority_queues[j];
            while (temp_queue != NULL) {
                if (temp_queue->task != next_task) {
                    temp_queue->task->waitingTime += slice;
                }
                temp_queue = temp_queue->next;
            }
        }

        // Aplica o Aging: verifica e aumenta a prioridade das tarefas que estão esperando por muito tempo
        for (int j = 0; j < MAX_PRIORITY_PA; j++) {
            struct node *temp_queue = pa_priority_queues[j];
            struct node *prev_aging_node = NULL;

            while (temp_queue != NULL) {
                Task *aging_task = temp_queue->task;
                struct node *next_aging_node = temp_queue->next; // Salva o próximo antes de possivelmente mover

                if (aging_task->waitingTime >= AGING_THRESHOLD && aging_task->priority > MIN_PRIORITY_PA) {
                    printf("Aging: Tarefa [%s] prioridade aumentada de %d para %d. Tempo de espera: %d\n",
                           aging_task->name, aging_task->priority, aging_task->priority - 1, aging_task->waitingTime);
                    aging_task->priority--; // Aumenta a prioridade (diminui o número)
                    aging_task->waitingTime = 0; // Reseta o tempo de espera

                    // Remove a tarefa da fila atual
                    if (prev_aging_node == NULL) { // É o head da fila
                        pa_priority_queues[j] = next_aging_node;
                    } else {
                        prev_aging_node->next = next_aging_node;
                    }
                    temp_queue->next = NULL; // Desconecta o nó da fila antiga

                    // Insere a tarefa na nova fila de prioridade
                    insert(&pa_priority_queues[aging_task->priority - 1], aging_task);

                    // Não avançamos 'temp_queue' aqui, pois o nó foi movido.
                    // 'temp_queue' deve ser o 'next_aging_node' para continuar a iteração.
                    temp_queue = next_aging_node;
                } else {
                    prev_aging_node = temp_queue;
                    temp_queue = temp_queue->next;
                }
            }
        }

        // Se a tarefa não terminou, insere-a de volta no final da fila de sua prioridade atual
        if (next_task->burst > 0) {
            insert(&pa_priority_queues[next_task->priority - 1], next_task);
        } else {
            // Tarefa concluída
            next_task->completionTime = current_time_pa;
            next_task->turnaroundTime = next_task->completionTime - next_task->arrivalTime;
            printf("Tarefa [%s] concluída no tempo %d. Turnaround: %d, Resposta: %d, Espera: %d\n",
                   next_task->name, next_task->completionTime, next_task->turnaroundTime,
                   next_task->responseTime, next_task->waitingTime);
            // Libera a memória da tarefa e do nó
            free(next_task->name);
            free(next_task);
            free(task_node_to_remove); // Libera o nó que foi removido da lista
        }
    }

    printf("Escalonamento de Prioridade com Aging concluído. Tempo total: %d\n", current_time_pa);
}
