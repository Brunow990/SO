// schedulers_edf.c - Earliest Deadline First Scheduler Implementation
#include "schedulers_edf.h"
#include "list.h"
#include "CPU.h"
#include <pthread.h>
#include <limits.h>
#include <stdlib.h>