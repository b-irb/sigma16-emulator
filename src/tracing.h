#pragma once
#include "events.h"
#include "instructions.h"
#include "vm.h"

#define ANSI_OFF "\x1b[0m"
#define ANSI_BLACK "\x1b[30m"
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_WHITE "\x1b[37m"

void dump_cpu(sigma16_cpu_t*);
void dump_vm_mem(sigma16_vm_t*, size_t, size_t);
/* user defined trace handler */
void sigma16_trace(sigma16_vm_t*, enum sigma16_trace_event);
