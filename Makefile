# makefile para o programa de escalonamento
#
# make rr    - para escalonamento Round-Robin
# make rr_p  - para escalonamento Round-Robin com prioridade
# make edf   - para escalonamento EDF
# make pa    - para escalonamento Prioridade com Aging

CC=gcc
CFLAGS=-Wall -pthread # Adiciona -pthread para futura implementação de threads, se necessário

clean:
	rm -rf *.o
	rm -rf rr
	rm -rf rr_p
	rm -rf edf
	rm -rf pa

# Regra para o escalonador Round-Robin (RR)
rr: driver.o list.o CPU.o schedule_rr.o
	$(CC) $(CFLAGS) -o rr driver.o schedule_rr.o list.o CPU.o

# Regra para o escalonador Round-Robin com Prioridade (RR_p)
rr_p: driver.o list.o CPU.o schedule_rr_p.o
	$(CC) $(CFLAGS) -o rr_p driver.o schedule_rr_p.o list.o CPU.o

# Regra para o escalonador Earliest Deadline First (EDF)
edf: driver.o list.o CPU.o schedule_edf.o
	$(CC) $(CFLAGS) -o edf driver.o schedule_edf.o list.o CPU.o

# Regra para o escalonador Prioridade com Aging (PA)
pa: driver.o list.o CPU.o schedule_pa.o
	$(CC) $(CFLAGS) -o pa driver.o schedule_pa.o list.o CPU.o

# Regras de compilação de objetos
driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c

schedule_rr.o: schedule_rr.c
	$(CC) $(CFLAGS) -c schedule_rr.c

schedule_rr_p.o: schedule_rr_p.c
	$(CC) $(CFLAGS) -c schedule_rr_p.c

schedule_edf.o: schedule_edf.c
	$(CC) $(CFLAGS) -c schedule_edf.c

schedule_pa.o: schedule_pa.c
	$(CC) $(CFLAGS) -c schedule_pa.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

CPU.o: CPU.c CPU.h
	$(CC) $(CFLAGS) -c CPU.c
