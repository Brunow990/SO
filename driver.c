/**
 * Driver.c
 *
 * A agenda está no formato
 *
 * [nome] [prioridade] [burst de CPU]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
// Inclui cabeçalhos para todos os escalonadores
#include "schedule_rr.h"
#include "schedule_rr_p.h"
#include "schedule_edf.h"
#include "schedule_pa.h" // Prioridade com Envelhecimento

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task_line[SIZE]; // Renomeado 'task' para 'task_line' para evitar conflito com a struct Task

    char *name;
    int priority;
    int burst;
    int deadline; // Adicionado para EDF

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    in = fopen(argv[1],"r");
    if (in == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    
    while (fgets(task_line, SIZE, in) != NULL) {
        temp = strdup(task_line); // Duplica a linha para tokenização
        if (temp == NULL) {
            perror("strdup falhou");
            exit(EXIT_FAILURE);
        }

        name = strsep(&temp,",");
        if (name == NULL) { free(temp); continue; }

        char *priority_str = strsep(&temp,",");
        if (priority_str == NULL) { free(temp); continue; }
        priority = atoi(priority_str);

        char *burst_str = strsep(&temp,",");
        if (burst_str == NULL) { free(temp); continue; }
        burst = atoi(burst_str);
        
        char *deadline_str = strsep(&temp, ","); // Para EDF, tenta ler o deadline
        if (deadline_str != NULL) {
            deadline = atoi(deadline_str);
            // Isso assume que o arquivo de entrada sempre tem um deadline se o EDF for o pretendido.
            // Em um cenário real, você pode querer um argumento de linha de comando para selecionar o escalonador.
            add_edf(name, priority, burst, deadline); // Chama a função add específica do EDF
        } else {
            // Se não houver deadline, assume que é para RR, RR_P ou PA
            // Você precisará escolher qual função 'add' chamar com base no escalonador desejado
            // Por exemplo, se você quiser testar RR_P:
            add_rr_p(name, priority, burst); 
            // Ou para RR: add_rr(name, priority, burst);
            // Ou para PA: add_pa(name, priority, burst);
        }

        free(temp); // Libera a string duplicada
    }

    fclose(in);

    // Invoca o escalonador. Você precisará selecionar qual escalonador executar.
    // Por exemplo, para executar EDF:
    //printf("\n--- Executando Escalondador EDF ---\n");
    //schedule_edf();
    
    // Para executar RR_P:
    // printf("\n--- Executando Escalondador Round Robin com Prioridade ---\n");
    // schedule_rr_p();

    // Para executar RR:
     printf("\n--- Executando Escalondador Round Robin ---\n");
     schedule_rr();

    // Para executar PA:
    // printf("\n--- Executando Escalondador Prioridade com Envelhecimento ---\n");
    // schedule_pa();


    return 0;
}