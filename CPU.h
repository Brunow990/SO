
// cpu.h
#ifndef CPU_H
#define CPU_H

#include "task.h" // Inclui a estrutura Task
#include "CPU.h"
// run the specified task for the following time slice
void run(Task *task, int slice);

#endif // CPU_H
