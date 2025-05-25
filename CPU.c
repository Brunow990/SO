/**
 * CPU "Virtual" que também mantém o controle do tempo do sistema.
 */

#include <stdio.h>
#include "task.h" // Inclui a definição da Task

// executa esta tarefa pela fatia de tempo especificada
void run(Task *task, int slice) {
    // Imprime informações da tarefa, incluindo o burst restante e o deadline.
    // O 'burst' aqui é o tempo restante da tarefa.
    printf("Executando tarefa = [%s] [Prioridade: %d] [Burst: %d] [Deadline: %d] por %d unidades.\n",
           task->name, task->priority, task->burst, task->deadline, slice);
}