/**
 * Various list operations using direct head pointer management
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"



Task* create_task(const char* name, int tid, int priority, int burst, int deadline, int waiting_time) {
    Task *newTask = (Task*) malloc(sizeof(Task));
    if (!newTask) {
        perror("Erro ao alocar memória para Task");
        exit(EXIT_FAILURE);
    }
    newTask->name = strdup(name); // copia a string para evitar problemas
    newTask->tid = tid;
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->deadline = deadline;
    newTask->waiting_time = waiting_time;
    return newTask;
}

// Função auxiliar para liberar uma Task
void free_task(Task *task) {
    if (task) {
        free(task->name);
        free(task);
    }
}

// Insere um novo nó no início da lista (comportamento de pilha)
void insert_at_head(struct node **head, Task *newTask) {
    struct node *newNode = (struct node*) malloc(sizeof(struct node));
    if (newNode == NULL) {
        perror("Erro ao alocar memória para o nó");
        exit(EXIT_FAILURE);
    }
    newNode->task = newTask;
    newNode->next = *head; // O novo nó aponta para a antiga cabeça
    *head = newNode;       // A cabeça da lista agora é o novo nó
}

// Insere um novo nó no final da lista (comportamento FIFO)
void insert_at_tail(struct node **head, Task *newTask) {
    struct node *newNode = (struct node*) malloc(sizeof(struct node));
    if (newNode == NULL) {
        perror("Erro ao alocar memória para o nó");
        exit(EXIT_FAILURE);
    }
    newNode->task = newTask;
    newNode->next = NULL; // Novo nó sempre será o último

    if (*head == NULL) {
        // Se a lista está vazia, o novo nó se torna a cabeça
        *head = newNode;
    } else {
        // Percorre até o final da lista
        struct node *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode; // Adiciona o novo nó no final
    }
}

// Deleta a tarefa selecionada da lista
void delete_task(struct node **head, Task *task) {
    if (*head == NULL) {
        return; // Lista vazia
    }

    struct node *temp = *head;
    struct node *prev = NULL;

    // Caso especial: a tarefa a ser deletada é a primeira da lista
    if (strcmp(task->name, temp->task->name) == 0) {
        *head = temp->next; // A cabeça da lista avança
        free(temp);         // Libera a memória do nó
        return;
    }

    // Procura a tarefa na lista
    prev = temp;
    temp = temp->next;
    while (temp != NULL && strcmp(task->name, temp->task->name) != 0) {
        prev = temp;
        temp = temp->next;
    }

    // Se a tarefa foi encontrada
    if (temp != NULL) {
        prev->next = temp->next; // O nó anterior aponta para o próximo do nó a ser deletado
        free(temp);             // Libera a memória do nó
    }
    // else: tarefa não encontrada na lista, não faz nada
}

// Pega a primeira tarefa da lista sem removê-la
Task* get_first_task(struct node *head) {
    if (head == NULL) {
        return NULL; // Lista vazia
    }
    return head->task;
}

// Remove e retorna a primeira tarefa da lista
Task* remove_first_task(struct node **head) {
    if (*head == NULL) {
        return NULL; // Lista vazia
    }
    struct node *temp = *head; // Guarda o nó da cabeça
    Task *task = temp->task;   // Pega a tarefa do nó da cabeça
    *head = temp->next;        // A cabeça da lista avança
    free(temp);                // Libera a memória do nó removido
    return task;               // Retorna a tarefa
}

// Percorre e imprime os elementos da lista
void traverse_list(struct node *head) {
    if (head == NULL) {
        printf("Lista vazia.\n");
        return;
    }
    struct node *temp = head;
    while (temp != NULL) {
        printf("[%s] [%d] [%d] [%d] [%d]\n",
               temp->task->name,
               temp->task->priority,
               temp->task->burst,
               temp->task->deadline,
               temp->task->waiting_time);
        temp = temp->next;
    }
}