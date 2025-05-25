/**
 * Driver.c
 *
 * A agenda está no formato:
 * [nome],[prioridade],[burst de CPU],[deadline (opcional para EDF)]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
// Inclui cabeçalhos específicos para cada escalonador
#include "schedule_rr.h"
#include "schedule_rr_p.h"
#include "schedule_edf.h"
#include "schedule_pa.h" // Prioridade com Envelhecimento

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp_line; // Usado para duplicar a linha lida
    char task_line[SIZE]; // Armazena a linha lida do arquivo

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
        temp_line = strdup(task_line); // Duplica a linha para tokenização segura
        if (temp_line == NULL) {
            perror("strdup falhou");
            exit(EXIT_FAILURE);
        }

        // Tokeniza a linha para extrair os atributos da tarefa
        name = strsep(&temp_line,",");
        if (name == NULL) { free(temp_line); continue; }

        char *priority_str = strsep(&temp_line,",");
        if (priority_str == NULL) { free(temp_line); continue; }
        priority = atoi(priority_str);

        char *burst_str = strsep(&temp_line,",");
        if (burst_str == NULL) { free(temp_line); continue; }
        burst = atoi(burst_str);
        
        char *deadline_str = strsep(&temp_line, ","); // Tenta ler o deadline
        
        // Decida qual função 'add' chamar com base no escalonador que você quer testar
        // e no formato da linha (se tem deadline ou não).

        // Exemplo: Se você está testando EDF, use add_edf.
        // Se o arquivo de entrada contém deadline, add_edf é o mais apropriado.
        if (deadline_str != NULL) {
            deadline = atoi(deadline_str);
            add_edf(name, priority, burst, deadline); 
        } else {
            // Se não houver deadline na linha, assuma que é para RR, RR_P ou PA.
            // Escolha APENAS UM dos 'add_' abaixo para as tarefas sem deadline.
            add_rr(name, priority, burst); // Para testar RR
            // add_rr_p(name, priority, burst); // Para testar RR_P
            // add_pa(name, priority, burst);   // Para testar PA
        }

        free(temp_line); // Libera a string duplicada
    }

    fclose(in);

    // Invoca o escalonador. Você precisará selecionar qual escalonador executar.
    // Descomente APENAS UM dos blocos abaixo para testar o escalonador desejado.

    // Para executar Round Robin (RR):
    printf("\n--- Executando Escalondador Round Robin ---\n");
    schedule_rr();
    
    // Para executar Round Robin com Prioridade (RR_p):
    // printf("\n--- Executando Escalondador Round Robin com Prioridade ---\n");
    // schedule_rr_p();

    // Para executar Earliest Deadline First (EDF):
    // printf("\n--- Executando Escalondador EDF ---\n");
    // schedule_edf();
    
    // Para executar Prioridade com Envelhecimento (PA):
    // printf("\n--- Executando Escalondador Prioridade com Envelhecimento ---\n");
    // schedule_pa();

    return 0;
}