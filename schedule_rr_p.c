#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedule_rr_p.h"
#include "task.h"
#include "list.h"
#include "CPU.h" // Para a função run

// Array de ponteiros de cabeçalho para filas de prioridade (MAX_PRIORITY a MIN_PRIORITY)
// Por exemplo, priority_queues[0] pode ser para MAX_PRIORITY, priority_queues[9] para MIN_PRIORITY
#define NUM_PRIORITY_LEVELS (MAX_PRIORITY - MIN_PRIORITY + 1)
struct node *priority_queues[NUM_PRIORITY_LEVELS];

static int rr_p_tid_counter = 0; // Contador global para IDs de tarefas
static int rr_p_global_time = 0; // Tempo global para RR_P

// Inicializa as filas de prioridade (deve ser chamada uma vez, por exemplo, em add ou schedule)
void initialize_priority_queues() {
    static int initialized = 0;
    if (!initialized) {
        for (int i = 0; i < NUM_PRIORITY_LEVELS; i++) {
            priority_queues[i] = NULL;
        }
        initialized = 1;
    }
}

// Função para escolher a próxima tarefa com base em Round Robin com Prioridade
Task *pickNextTask_rr_p() {
    initialize_priority_queues();

    // Itera da prioridade mais alta (1) para a mais baixa (10)
    for (int i = 0; i < NUM_PRIORITY_LEVELS; i++) {
        // O índice `i` corresponde à prioridade `MIN_PRIORITY + i`
        // Então, se MIN_PRIORITY é 1, o índice 0 é a prioridade 1, o índice 9 é a prioridade 10.
        if (priority_queues[i] != NULL) {
            // Encontrou uma tarefa na fila de prioridade mais alta disponível
            // Para o comportamento Round Robin dentro da mesma prioridade, pega o cabeçalho.
            return priority_queues[i]->task;
        }
    }
    return NULL; // Nenhuma tarefa em nenhuma fila
}

// adiciona uma tarefa à lista 
void add_rr_p(char *name, int priority, int burst){
    initialize_priority_queues();

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
    newTask->tid = ++rr_p_tid_counter;
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->initial_burst = burst; // Armazena o burst inicial
    newTask->arrival_time = rr_p_global_time; // Registra o tempo de chegada
    newTask->last_run_time = rr_p_global_time; // Inicializa o tempo da última execução

    // Insere na fila de prioridade correta (cabeçalho da lista por enquanto)
    // Ajusta o índice com base em MIN_PRIORITY (ex: prioridade 1 vai para o índice 0)
    insert(&priority_queues[priority - MIN_PRIORITY], newTask);
    printf("Tarefa adicionada [%s] prioridade [%d] burst [%d]\n", name, priority, burst);
}

// invoca o escalonador
void schedule_rr_p(){
    initialize_priority_queues();
    
    // Verifica se há alguma tarefa em alguma fila
    int tasks_remaining = 1; // Assume que há tarefas restantes até que se prove o contrário
    while (tasks_remaining) {
        tasks_remaining = 0; // Reseta para esta iteração

        for (int i = 0; i < NUM_PRIORITY_LEVELS; i++) {
            // Verifica se esta fila de prioridade tem tarefas
            if (priority_queues[i] != NULL) {
                tasks_remaining = 1; // Temos tarefas, continua escalonando
                
                // Pega a tarefa no cabeçalho da fila de prioridade atual
                Task *current_task = priority_queues[i]->task;

                // Remove a tarefa do cabeçalho (para reinserir no final se não estiver concluída)
                delete(&priority_queues[i], current_task); 

                // Executa a tarefa por um quantum ou seu burst restante, o que for menor
                int slice = (current_task->burst < QUANTUM) ? current_task->burst : QUANTUM;
                
                run(current_task, slice); // Simula a execução da tarefa
                current_task->burst -= slice; // Diminui o burst restante
                rr_p_global_time += slice; // Avança o tempo global

                current_task->last_run_time = rr_p_global_time; // Atualiza o tempo da última execução

                // Se a tarefa não estiver concluída, reinserir no final de sua fila de prioridade
                if (current_task->burst > 0) {
                    // Reinserir no final para simular Round Robin dentro da mesma prioridade
                    // A função 'insert' adiciona ao cabeçalho, então precisamos percorrer até o final.
                    struct node *temp = priority_queues[i];
                    if (temp == NULL) { // Se a fila estava vazia após a exclusão
                        insert(&priority_queues[i], current_task);
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
                    // A memória da tarefa já foi liberada pela função 'delete' (se implementado para isso)
                    // Caso contrário, garanta que seja liberada aqui:
                    // free(current_task->name);
                    // free(current_task);
                }
                // Sai deste loop e reavalia para a próxima tarefa de maior prioridade
                // Isso garante que a prioridade seja respeitada em cada decisão de escalonamento.
                break; 
            }
        }
    }
    printf("Escalonamento Round Robin com Prioridade completo. Tempo total: %d\n", rr_p_global_time);
}