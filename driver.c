/**
 * Driver.c
 *
 * O arquivo de agendamento está no formato
 *
 * [nome] [prioridade] [burst da CPU] [deadline (opcional)]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // Para usar bool

#include "task.h"
#include "list.h"
#include "CPU.h"

// Inclui os cabeçalhos de TODOS os escalonadores
#include "schedule_rr.h"
#include "schedule_rr_p.h"
#include "schedule_edf.h"
#include "schedule_pa.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task_line[SIZE]; // Buffer para ler a linha da tarefa

    char *name;
    int priority;
    int burst;
    int deadline = 0; // Inicializa deadline como 0 (opcional, usado apenas para EDF/PA)

    // Verifica o número de argumentos da linha de comando
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <arquivo_tarefas> <escalonador>\n", argv[0]);
        fprintf(stderr, "Escalonadores disponíveis: rr, rr_p, edf, pa\n");
        return 1;
    }

    // Abre o arquivo de entrada
    in = fopen(argv[1], "r");
    if (in == NULL) {
        perror("Erro ao abrir o arquivo de tarefas");
        return 1;
    }

    char *scheduler_type = argv[2]; // Tipo de escalonador a ser usado

    // Loop para ler as tarefas do arquivo
    while (fgets(task_line, SIZE, in) != NULL) {
        temp = strdup(task_line); // Duplica a linha para tokenização
        if (temp == NULL) {
            perror("Falha ao duplicar a string da tarefa");
            fclose(in);
            return 1;
        }

        // Extrai nome, prioridade e burst
        name = strsep(&temp, ",");
        priority = atoi(strsep(&temp, ","));
        burst = atoi(strsep(&temp, ","));

        // Se o escalonador for EDF ou Prioridade com Aging, tenta ler o deadline
        if (strcmp(scheduler_type, "edf") == 0 || strcmp(scheduler_type, "pa") == 0) {
            char *deadline_str = strsep(&temp, ",");
            if (deadline_str != NULL) {
                deadline = atoi(deadline_str);
            } else {
                fprintf(stderr, "Aviso: Deadline não especificado para tarefa %s no modo %s. Usando 0.\n", name, scheduler_type);
                deadline = 0; // Define um valor padrão se não houver deadline
            }
        }

        // Adiciona a tarefa ao escalonador apropriado
        if (strcmp(scheduler_type, "rr") == 0) {
            add_rr(name, priority, burst);
        } else if (strcmp(scheduler_type, "rr_p") == 0) {
            add_rr_p(name, priority, burst);
        } else if (strcmp(scheduler_type, "edf") == 0) {
            add_edf(name, priority, burst, deadline);
        } else if (strcmp(scheduler_type, "pa") == 0) {
            add_pa(name, priority, burst);
        } else {
            fprintf(stderr, "Erro: Escalonador '%s' desconhecido.\n", scheduler_type);
            free(temp); // Libera o temp original antes de sair
            fclose(in);
            return 1;
        }

        free(temp); // Libera a cópia da linha
    }

    fclose(in); // Fecha o arquivo de entrada

    // Invoca o escalonador selecionado
    if (strcmp(scheduler_type, "rr") == 0) {
        schedule_rr();
    } else if (strcmp(scheduler_type, "rr_p") == 0) {
        schedule_rr_p();
    } else if (strcmp(scheduler_type, "edf") == 0) {
        schedule_edf();
    } else if (strcmp(scheduler_type, "pa") == 0) {
        schedule_pa();
    } else {
        // Já tratado acima, mas para segurança
        fprintf(stderr, "Erro interno: Escalonador '%s' não invocado.\n", scheduler_type);
        return 1;
    }

    return 0;
}
