/**
 * Várias operações de lista encadeada.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"


// adiciona uma nova tarefa ao início da lista de tarefas
void insert(struct node **head, Task *newTask) {
    struct node *newNode = malloc(sizeof(struct node));
    if (newNode == NULL) {
        perror("Falha ao alocar novo nó para a lista");
        exit(EXIT_FAILURE);
    }

    newNode->task = newTask;
    newNode->next = *head;
    *head = newNode;
}

// remove a tarefa selecionada da lista e libera sua memória
void delete(struct node **head, Task *task) {
    struct node *temp;
    struct node *prev;

    temp = *head;
    // Caso especial - tarefa no início da lista
    if (temp != NULL && strcmp(task->name, temp->task->name) == 0) {
        *head = (*head)->next;
        free(temp->task->name); // Libera a string do nome da tarefa
        free(temp->task); // Libera a estrutura da tarefa
        free(temp); // Libera o nó da lista
        return;
    }
    
    // Percorre a lista para encontrar a tarefa
    prev = *head;
    if (prev != NULL) {
        temp = prev->next;
        while (temp != NULL && strcmp(task->name, temp->task->name) != 0) {
            prev = temp;
            temp = temp->next;
        }

        if (temp != NULL) { // Se a tarefa foi encontrada
            prev->next = temp->next;
            free(temp->task->name); // Libera a string do nome da tarefa
            free(temp->task); // Libera a estrutura da tarefa
            free(temp); // Libera o nó da lista
        }
    }
}

// percorre a lista e imprime os detalhes das tarefas
void traverse(struct node *head) {
    struct node *temp;
    temp = head;

    while (temp != NULL) {
        printf("[%s] [Prioridade: %d] [Burst: %d] [Deadline: %d]\n", temp->task->name, temp->task->priority, temp->task->burst, temp->task->deadline);
        temp = temp->next;
    }
}