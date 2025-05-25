// driver.c
/**
 * Driver.c
 *
 * Lê as tarefas de um arquivo de entrada e as adiciona ao escalonador selecionado.
 * Em seguida, invoca a função de escalonamento.
 *
 * Formato do arquivo de entrada:
 * [name],[priority],[CPU burst] (para RR, RR_p, Aging)
 * [name],[priority],[CPU burst],[deadline] (para EDF)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Para strdup, strsep, strchr

#include "task.h" // Define a estrutura Task
#include "list.h" // Funções para manipulação de lista encadeada
#include "CPU.h"  // Função para simular a execução da CPU

// As macros (USE_RR_SCHEDULER, USE_RR_P_SCHEDULER, etc.) serão definidas no Makefile.
// Isso permite que o driver.c inclua o cabeçalho correto e chame a função add() com os parâmetros certos.
#ifdef USE_RR_SCHEDULER
    #include "schedulers_rr.h"
#elif defined USE_RR_P_SCHEDULER
    #include "schedulers_rr_p.h"
#elif defined USE_EDF_SCHEDULER
    #include "schedulers_edf.h"
#elif defined USE_PA_SCHEDULER
    #include "schedulers_pa.h"
#endif

#define SIZE    100 // Tamanho máximo da linha lida do arquivo

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp_line_copy; // Usado para strdup e strsep
    char task_line_buffer[SIZE]; // Buffer para ler a linha do arquivo

    char *name;
    int priority;
    int burst;
    int deadline = 0; // Inicializado para 0, para escalonadores que não usam deadline

    // Verifica se o nome do arquivo de entrada foi fornecido como argumento de linha de comando
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <nome_do_arquivo_de_tarefas>\n", argv[0]);
        return 1; // Retorna código de erro
    }

    // Tenta abrir o arquivo de entrada para leitura
    in = fopen(argv[1], "r");
    if (in == NULL) {
        perror("Erro ao abrir o arquivo de tarefas"); // Imprime mensagem de erro do sistema
        return 1;
    }
    
    // Lê o arquivo linha por linha
    while (fgets(task_line_buffer, SIZE, in) != NULL) {
        // strdup duplica a string, pois strsep modifica a string original.
        // Precisamos liberar esta memória depois.
        temp_line_copy = strdup(task_line_buffer);
        if (temp_line_copy == NULL) {
            perror("Erro ao alocar memória para a linha da task");
            fclose(in);
            exit(EXIT_FAILURE);
        }
        
        // Remove o caractere de nova linha (\n) que fgets pode ler
        char *newline = strchr(temp_line_copy, '\n');
        if (newline) {
            *newline = '\0';
        }

        char *parse_ptr = temp_line_copy; // Ponteiro auxiliar para strsep

        // Extrai os campos da linha usando strsep
        name = strsep(&parse_ptr, ",");
        priority = atoi(strsep(&parse_ptr, ","));
        burst = atoi(strsep(&parse_ptr, ","));
        
        // Tenta ler o deadline. Se parse_ptr for NULL, significa que não há mais campos.
        // Isso permite que o mesmo driver.c leia arquivos com ou sem deadline.
        char *deadline_str = strsep(&parse_ptr, ",");
        if (deadline_str != NULL) {
            deadline = atoi(deadline_str);
        } else {
            deadline = 0; // Define 0 se não houver deadline no arquivo
        }

        // Chama a função 'add' do escalonador atualmente compilado.
        // As macros USE_EDF_SCHEDULER etc. controlam qual versão de 'add' é chamada.
        #ifdef USE_EDF_SCHEDULER
            // Para EDF, 'add' espera o parâmetro deadline
            add(name, priority, burst, deadline);
        #else
            // Para RR, RR_p, PA, 'add' espera 3 parâmetros.
            // O valor de 'deadline' será ignorado por esses escalonadores,
            // mas pode ser armazenado na Task struct.
            add(name, priority, burst);
        #endif

        free(temp_line_copy); // Libera a memória alocada por strdup
    }

    fclose(in); // Fecha o arquivo de entrada

    // Invoca a função principal de escalonamento.
    // Esta chamada será resolvida para a função schedule() do escalonador
    // cujo cabeçalho foi incluído.
    schedule();

    return 0; // Indica sucesso
}