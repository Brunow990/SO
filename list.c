/**
 * Várias operações de lista
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"


// adiciona uma nova tarefa à lista de tarefas
void insert(struct node **head, Task *newTask) {
    // adiciona a nova tarefa à lista 
    struct node *newNode = malloc(sizeof(struct node));
    if (newNode == NULL) {
        perror("Falha ao alocar novo nó");
        exit(EXIT_FAILURE);
    }

    newNode->task = newTask;
    newNode->next = *head;
    *head = newNode;
}

// remove a tarefa selecionada da lista
void delete(struct node **head, Task *task) {
    struct node *temp;
    struct node *prev;

    temp = *head;
    // caso especial - início da lista
    if (temp != NULL && strcmp(task->name,temp->task->name) == 0) {
        *head = (*head)->next;
        free(temp->task->name); // Libera o nome da tarefa
        free(temp->task); // Libera a struct da tarefa
        free(temp); // Libera o nó
        return;
    }
    
    // elemento interior ou último da lista
    prev = *head;
    if (prev != NULL) { // Garante que prev não seja NULL antes de acessar prev->next
        temp = prev->next;
        while (temp != NULL && strcmp(task->name,temp->task->name) != 0) {
            prev = temp;
            temp = temp->next;
        }

        if (temp != NULL) { // Encontrou a tarefa para remover
            prev->next = temp->next;
            free(temp->task->name); // Libera o nome da tarefa
            free(temp->task); // Libera a struct da tarefa
            free(temp); // Libera o nó
        }
    }
}

// percorre a lista
void traverse(struct node *head) {
    struct node *temp;
    temp = head;

    while (temp != NULL) {
        printf("[%s] [%d] [%d] [Deadline: %d]\n", temp->task->name, temp->task->priority, temp->task->burst, temp->task->deadline);
        temp = temp->next;
    }
}