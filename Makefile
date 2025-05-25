# makefile for scheduling program

CC=gcc
CFLAGS=-Wall -pthread

clean:
	rm -rf *.o
	rm -rf rr
	rm -rf rr_p
	rm -rf edf
	rm -rf pa

# =========================================================
# Alvos para cada tipo de escalonador (ajustados para seus nomes de arquivo)
# =========================================================

# Alvo para Round-Robin Normal (RR)
rr: CFLAGS += -DUSE_RR_SCHEDULER
rr: driver.o list.o CPU.o schedulers_rr.o # <-- Nome do objeto ajustado
	$(CC) $(CFLAGS) -o rr driver.o schedulers_rr.o list.o CPU.o # <-- Nome do objeto ajustado

# Alvo para Round-Robin com Prioridade (RR_p)
rr_p: CFLAGS += -DUSE_RR_P_SCHEDULER
rr_p: driver.o list.o CPU.o schedulers_rr_p.o # <-- Nome do objeto ajustado
	$(CC) $(CFLAGS) -o rr_p driver.o schedulers_rr_p.o list.o CPU.o # <-- Nome do objeto ajustado

# Alvo para Earliest Deadline First (EDF)
edf: CFLAGS += -DUSE_EDF_SCHEDULER
edf: driver.o list.o CPU.o schedulers_edf.o # <-- Nome do objeto ajustado
	$(CC) $(CFLAGS) -o edf driver.o schedulers_edf.o list.o CPU.o # <-- Nome do objeto ajustado

# Alvo para Prioridade com Aging (PA) - Assumindo que você tem schedulers_aging.c/h
pa: CFLAGS += -DUSE_PA_SCHEDULER
pa: driver.o list.o CPU.o schedulers_aging.o # <-- Nome do objeto ajustado (se for aging.c)
	$(CC) $(CFLAGS) -o pa driver.o schedulers_aging.o list.o CPU.o # <-- Nome do objeto ajustado

# =========================================================
# Regras de compilação de arquivos objeto (.o) - Ajustadas para seus nomes de arquivo
# =========================================================

driver.o: driver.c task.h list.h CPU.h # <-- CPU.h com 'C' maiúsculo
	$(CC) $(CFLAGS) -c driver.c

# Ajuste para o nome do arquivo schedulers_rr.c
schedulers_rr.o: schedulers_rr.c schedulers_rr.h task.h list.h CPU.h
	$(CC) $(CFLAGS) -c schedulers_rr.c

# Ajuste para o nome do arquivo schedulers_rr_p.c
schedulers_rr_p.o: schedulers_rr_p.c schedulers_rr_p.h task.h list.h CPU.h
	$(CC) $(CFLAGS) -c schedulers_rr_p.c

# Ajuste para o nome do arquivo schedulers_edf.c
schedulers_edf.o: schedulers_edf.c schedulers_edf.h task.h list.h CPU.h
	$(CC) $(CFLAGS) -c schedulers_edf.c

# Ajuste para o nome do arquivo schedulers_aging.c (se for o caso)
schedulers_aging.o: schedulers_aging.c schedulers_aging.h task.h list.h CPU.h
	$(CC) $(CFLAGS) -c schedulers_aging.c

list.o: list.c list.h task.h
	$(CC) $(CFLAGS) -c list.c

CPU.o: CPU.c CPU.h task.h # <-- CPU.c e CPU.h com 'C' maiúsculo
	$(CC) $(CFLAGS) -c CPU.c