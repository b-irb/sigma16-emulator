#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

typedef struct _sigma16_vm {
    sigma16_cpu_t cpu;
    uint16_t* mem;
} sigma16_vm_t;

int sigma16_vm_init(sigma16_vm_t**, char*);
void sigma16_vm_del(sigma16_vm_t*);
int sigma16_vm_exec(sigma16_vm_t*);
