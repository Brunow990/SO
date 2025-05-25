/**
 * Estrutura de dados de lista encadeada contendo as tarefas no sistema.
 */

#include "task.h" // Inclui a definição da Task

struct node {
    Task *task;
    struct node *next;
};

// operações de inserção e exclusão.
void insert(struct node **head, Task *task);
void delete(struct node **head, Task *task);
void traverse(struct node *head);