#include <stdio.h>
#include "schedulers_rr.h"

// Função principal
int main() {
    printf("\n========================\n");
    printf("Teste Escalonador RR com Prioridade\n");
    printf("========================\n\n");

    // Adiciona tarefas com diferentes prioridades e bursts
    add("Tarefa1", 2, 10);   // Prioridade 2, burst 10
    add("Tarefa2", 1, 5);    // Prioridade 1 (mais alta), burst 5
    add("Tarefa3", 3, 8);    // Prioridade 3, burst 8
    add("Tarefa4", 2, 6);    // Prioridade 2, burst 6
    add("Tarefa5", 1, 4);    // Prioridade 1, burst 4

    printf("\n>>> Iniciando Escalonamento...\n");
    schedule();
    printf("\n>>> Escalonamento concluído.\n");

    return 0;
}