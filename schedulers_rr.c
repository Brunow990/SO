#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "task.h"
#include "CPU.h"
#include "schedulers_rr_p.h"

#define QUANTUM 2   // Defina aqui o quantum desejado


// Filas separadas por prioridade
struct node *fila = NULL;

// Gera IDs únicos para cada tarefa
int tid_counter = 1;

void add(char *name, int priority, int burst) {
    Task *task = create_task(name, tid_counter++,0,burst,0,0);
    insert_at_tail(&fila, task);
}

void schedule() {
    while (fila != NULL) {
        Task *tarefa = remove_first_task(&fila);

        int slice = (tarefa->burst > QUANTUM) ? QUANTUM : tarefa->burst;

        run(tarefa, slice);

        tarefa->burst -= slice;

        if (tarefa->burst > 0) {
            insert_at_tail(&fila, tarefa);
        } else {
            printf("Task [%s] (TID: %d) concluída.\n", tarefa->name, tarefa->tid);
            free_task(tarefa);
        }
    }
}