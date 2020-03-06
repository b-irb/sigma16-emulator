#pragma once
#include "vm.h"

void trace_rx(sigma16_vm_t*, char*);
void trace_rrr(sigma16_vm_t*, char*);
void trace_branch(sigma16_vm_t*, char*);
void trace_trap(sigma16_vm_t*);
void trace_cmp(sigma16_vm_t*);
