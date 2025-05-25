#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "schedule_rr.h"
#include "list.h"
#include "CPU.h"

// Lista de tarefas prontas para o escalonador Round Robin
struct node *rr_task_list = NULL;

// Variável global para o tempo atual do sistema
int current_time_rr = 0;

// Adiciona uma tarefa à lista de tarefas prontas para o escalonador Round Robin
void add_rr(char *name, int priority, int burst) {
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
    newTask->deadline = 0;          // Não usado para RR
    newTask->arrivalTime = current_time_rr; // Define o tempo de chegada
    newTask->waitingTime = 0;       // Inicializa o tempo de espera
    newTask->responseTime = -1;     // Inicializa o tempo de resposta como -1 (não iniciado)
    newTask->turnaroundTime = 0;
    newTask->completionTime = 0;
    newTask->started = 0;           // Tarefa ainda não iniciou

    // Insere a tarefa no final da lista (para simular uma fila FIFO)
    // Como a função insert existente adiciona no início, precisamos de uma forma de adicionar no final
    // Para simplificar, vamos manter a lógica de insert no início e o escalonador vai iterar.
    // Ou, podemos reverter a lista antes de cada iteração, mas isso seria ineficiente.
    // A melhor abordagem é ter um ponteiro para o final da lista ou iterar para encontrar o final.
    // Por enquanto, vamos usar a inserção no início e o escalonador vai "simular" FIFO.

    // Para manter a essência da lista como pilha (insert no início),
    // o escalonador RR precisará percorrer a lista para encontrar a tarefa mais antiga.
    // Uma abordagem mais simples seria ter um ponteiro para o último elemento para inserção O(1).
    // Mas, seguindo a instrução de "não perder a essência da implementação disponibilizada",
    // vamos usar a `insert` existente e gerenciar a ordem no `schedule_rr`.
    insert(&rr_task_list, newTask);
}

// Invoca o escalonador Round-Robin
void schedule_rr() {
    printf("Iniciando escalonamento Round Robin...\n");

    // Loop principal de escalonamento
    while (rr_task_list != NULL) {
        struct node *current_node = rr_task_list;
        struct node *prev_node = NULL;
        Task *current_task = NULL;

        // Encontra a próxima tarefa na fila (FIFO)
        // Como 'insert' adiciona no início, a tarefa mais antiga está no final da lista.
        // Precisamos percorrer a lista para encontrar a tarefa mais antiga.
        if (current_node == NULL) {
            break; // Nenhuma tarefa na lista
        }

        // Se a lista tiver apenas um elemento, ele é o próximo
        if (current_node->next == NULL) {
            current_task = current_node->task;
            // Remove o nó da lista temporariamente para re-inserir se não terminar
            rr_task_list = NULL; // A lista fica vazia
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
            break; // Não deveria acontecer se a lista não for nula
        }

        // Atualiza o tempo de resposta se a tarefa estiver começando pela primeira vez
        if (current_task->started == 0) {
            current_task->responseTime = current_time_rr;
            current_task->started = 1;
        }

        // Calcula o tempo de execução para este quantum
        int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;

        // Executa a tarefa
        run(current_task, slice);
        current_task->burst -= slice;
        current_time_rr += slice; // Avança o tempo do sistema

        // Atualiza o tempo de espera para todas as outras tarefas na fila
        struct node *temp = rr_task_list;
        while (temp != NULL) {
            if (temp->task != current_task) {
                temp->task->waitingTime += slice;
            }
            temp = temp->next;
        }

        // Se a tarefa não terminou, insere-a de volta no final da fila
        if (current_task->burst > 0) {
            // Re-insere no início e o próximo ciclo irá encontrá-la novamente no final
            insert(&rr_task_list, current_task);
        } else {
            // Tarefa concluída
            current_task->completionTime = current_time_rr;
            current_task->turnaroundTime = current_task->completionTime - current_task->arrivalTime;
            printf("Tarefa [%s] concluída no tempo %d. Turnaround: %d, Resposta: %d, Espera: %d\n",
                   current_task->name, current_task->completionTime, current_task->turnaroundTime,
                   current_task->responseTime, current_task->waitingTime);
            // Libera a memória da tarefa e do nó (já feito pela função delete_task_by_name)
            // Como removemos o nó manualmente, precisamos liberar aqui
            free(current_task->name);
            free(current_task);
            free(current_node); // Libera o nó que foi removido da lista
        }
    }

    printf("Escalonamento Round Robin concluído. Tempo total: %d\n", current_time_rr);
}
