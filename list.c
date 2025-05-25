/**
 * Operações diversas sobre listas encadeadas.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"


// Adiciona uma nova tarefa ao início da lista
void insert(struct node **head, Task *newTask) {
    // Aloca memória para o novo nó
    struct node *newNode = malloc(sizeof(struct node));
    if (newNode == NULL) {
        perror("Falha ao alocar memória para o novo nó da lista");
        exit(EXIT_FAILURE);
    }

    newNode->task = newTask;    // Atribui a tarefa ao nó
    newNode->next = *head;      // O próximo nó será o antigo início da lista
    *head = newNode;            // O novo nó se torna o início da lista
}

// Deleta a tarefa selecionada da lista (procurando pelo ponteiro da tarefa)
void delete(struct node **head, Task *task) {
    struct node *temp;
    struct node *prev;

    temp = *head;
    // Caso especial - início da lista
    if (temp != NULL && temp->task == task) {
        *head = temp->next;
        free(temp->task->name); // Libera a memória do nome da tarefa
        free(temp->task);       // Libera a memória da tarefa
        free(temp);             // Libera a memória do nó
        return;
    }

    // Percorre a lista para encontrar a tarefa
    while (temp != NULL && temp->task != task) {
        prev = temp;
        temp = temp->next;
    }

    // Se a tarefa não foi encontrada
    if (temp == NULL) {
        return;
    }

    // Remove o nó da lista
    prev->next = temp->next;
    free(temp->task->name); // Libera a memória do nome da tarefa
    free(temp->task);       // Libera a memória da tarefa
    free(temp);             // Libera a memória do nó
}

// Deleta a tarefa selecionada da lista (procurando pelo nome da tarefa)
void delete_task_by_name(struct node **head, char *name) {
    struct node *temp;
    struct node *prev = NULL;

    temp = *head;
    // Caso especial - início da lista
    if (temp != NULL && strcmp(temp->task->name, name) == 0) {
        *head = temp->next;
        free(temp->task->name); // Libera a memória do nome da tarefa
        free(temp->task);       // Libera a memória da tarefa
        free(temp);             // Libera a memória do nó
        return;
    }

    // Percorre a lista para encontrar a tarefa
    while (temp != NULL && strcmp(temp->task->name, name) != 0) {
        prev = temp;
        temp = temp->next;
    }

    // Se a tarefa não foi encontrada
    if (temp == NULL) {
        return;
    }

    // Remove o nó da lista
    if (prev != NULL) {
        prev->next = temp->next;
    }
    free(temp->task->name); // Libera a memória do nome da tarefa
    free(temp->task);       // Libera a memória da tarefa
    free(temp);             // Libera a memória do nó
}


// Percorre e imprime os detalhes de todas as tarefas na lista
void traverse(struct node *head) {
    struct node *temp;
    temp = head;

    while (temp != NULL) {
        printf("  [%s] Prioridade: %d, Burst: %d, Deadline: %d, Chegada: %d, Espera: %d, Resposta: %d, Turnaround: %d, Conclusao: %d\n",
               temp->task->name, temp->task->priority, temp->task->burst, temp->task->deadline,
               temp->task->arrivalTime, temp->task->waitingTime, temp->task->responseTime,
               temp->task->turnaroundTime, temp->task->completionTime);
        temp = temp->next;
    }
}
