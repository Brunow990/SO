
// cpu.c
/**
 * "Virtual" CPU that also maintains track of system time.
 */

#include <stdio.h> // Para printf

#include "CPU.h"   // Inclui o próprio cabeçalho
#include "task.h"  // Inclui a estrutura Task

// run this task for the specified time slice
void run(Task *task, int slice) {
    // Imprime o nome da tarefa, seu ID, a prioridade, o burst restante, e o slice a ser executado.
    printf("Running task [%s] (TID: %d, Priority: %d, Burst: %d) for %d units.\n",
           task->name, task->tid, task->priority, task->burst, slice);
    // Nota: O burst da tarefa (task->burst) DEVE ser atualizado pelo escalonador
    // (schedule_*.c) após a chamada desta função `run`, subtraindo o `slice` executado.
}
