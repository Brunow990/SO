#ifndef LIST_H
#define LIST_H

#include "task.h"

// Estrutura do nó da lista encadeada
struct node {
    Task *task;         // Ponteiro para a tarefa
    struct node *next;  // Ponteiro para o próximo nó
};

// Funções de inserção, remoção e travessia da lista
void insert(struct node **head, Task *newTask);
void delete(struct node **head, Task *task);
void delete_task_by_name(struct node **head, char *name); // Nova função para deletar por nome
void traverse(struct node *head);

#endif
