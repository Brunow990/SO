/**
 * CPU "Virtual" que também mantém o controle do tempo do sistema.
 */

#include <stdio.h>

#include "task.h"

// Executa esta tarefa pela fatia de tempo especificada
void run(Task *task, int slice) {
    printf("Executando tarefa = [%s] Prioridade: %d, Burst Restante: %d por %d unidades.\n",
           task->name, task->priority, task->burst, slice);
}
