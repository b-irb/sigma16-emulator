#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "cpu.h"
#include "instructions.h"

typedef struct _sigma16_vm {
    sigma16_cpu_t cpu;
    uint16_t* mem;
#ifdef ENABLE_TRACE
    void (*trace_handler)(struct _sigma16_vm*, enum sigma16_instruction_fmt);
#endif
#ifdef PYTHON_COMPAT
    void* py_obj_self;
#endif
} sigma16_vm_t;

int sigma16_vm_init(sigma16_vm_t**, char*);
void sigma16_vm_del(sigma16_vm_t*);
int sigma16_vm_exec(sigma16_vm_t*);
uint16_t read_mem(sigma16_vm_t*, uint16_t);
void write_mem(sigma16_vm_t*, uint16_t, uint16_t);
