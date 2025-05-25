// list.h
/**
 * list data structure containing the tasks in the system
 */

#ifndef LIST_H
#define LIST_H

#include "task.h" // Inclui a estrutura Task

struct node {
    Task *task;
    struct node *next;
};

// Funções de operação na lista.
// Todas agora recebem struct node **head para modificar a cabeça da lista.
void insert_at_head(struct node **head, Task *task); // Para o comportamento de pilha original
void insert_at_tail(struct node **head, Task *task); // Para comportamento FIFO (RR clássico)
void delete_task(struct node **head, Task *task); // Deleta uma tarefa específica
Task* get_first_task(struct node *head); // Pega a primeira tarefa sem remover
Task* remove_first_task(struct node **head); // Remove e retorna a primeira tarefa
void traverse_list(struct node *head); // Percorre e imprime a lista
Task* create_task(const char* name, int tid, int priority, int burst, int deadline, int waiting_time);
void free_task(Task *task);

#endif // LIST_H