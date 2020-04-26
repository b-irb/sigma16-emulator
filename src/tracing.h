#pragma once
#include "instructions.h"
#include "vm.h"

/* user defined trace handler */
void sigma16_trace(sigma16_vm_t*, enum sigma16_instruction_fmt);
