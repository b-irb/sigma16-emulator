#include <stdio.h>

#include "instructions.h"
#include "vm.h"

const static char* RRR_INST_MNEMONICS[] = {
    "add",   "sub", "mul", "div", "cmp", "cmplt", "cmpeq",
    "cmpgt", "inv", "and", "or",  "xor", "nop",   "trap"};

const static char* RX_INST_MNEMONICS[] = {"lea",   "load",   "store",
                                          "jump",  "jumpc0", "jumpc1",
                                          "jumpf", "jumpt",  "jal"};

const static char* EXP_INST_MNEMONICS[] = {"rfi"};

void trace_rx(sigma16_vm_t*, const char*);
void trace_rrr(sigma16_vm_t*, const char*);
void trace_branch(sigma16_vm_t*, const char*);
void trace_trap(sigma16_vm_t*);
void trace_cmp(sigma16_vm_t*);
void dump_cpu(sigma16_cpu_t*);

void sigma16_trace(sigma16_vm_t* vm, enum sigma16_instruction_fmt fmt) {
    switch (fmt) {
        case RRR:
            trace_rrr(vm, RRR_INST_MNEMONICS[vm->cpu.ir.rrr.op]);
            break;
        case RX:
            trace_rx(vm, RX_INST_MNEMONICS[vm->cpu.ir.rx.d]);
            break;
        case EXP0:
            /* TODO EXP */
            break;
    }
}

void trace_rx(sigma16_vm_t* vm, const char* inst_name) {
    printf("[%04x]\t\x1b[32m%s\t\x1b[31mR%d\x1b[0m, ", vm->cpu.pc, inst_name,
           vm->cpu.ir.rx.d);

    if (vm->cpu.ir.rx.disp) {
        printf("\x1b[33m0x%02x\x1b[0m", vm->cpu.ir.rx.disp);
    } else {
        printf("0x%4x", vm->cpu.ir.rx.disp);
    }

    printf("[\x1b[31mR%d\x1b[0m]\n", vm->cpu.ir.rx.sa);
}

void trace_rrr(sigma16_vm_t* vm, const char* inst_name) {
    printf(
        "[%04x]\t\x1b[34m%s\t\x1b[31mR%d\x1b[0m, \x1b[31mR%d\x1b[0m,\x1b[31m "
        "R%d\x1b[0m\n",
        vm->cpu.pc, inst_name, vm->cpu.ir.rrr.d, vm->cpu.ir.rrr.sa,
        vm->cpu.ir.rrr.sb);
}

void trace_branch(sigma16_vm_t* vm, const char* inst_name) {
    printf("[%04x]\t\x1b[33m%s\t\x1b[33m0x%02x\x1b[0m[\x1b[31mR%d\x1b[0m]\n",
           vm->cpu.pc, inst_name, vm->cpu.ir.rx.disp, vm->cpu.ir.rx.sa);
}

void trace_trap(sigma16_vm_t* vm) {
    printf("[%04x]\t\x1b[35mtrap\t\x1b[31mR%d, R%d, R%d\x1b[0m\n", vm->cpu.pc,
           vm->cpu.ir.rrr.d, vm->cpu.ir.rrr.sa, vm->cpu.ir.rrr.sb);
}

void trace_cmp(sigma16_vm_t* vm) {
    printf("[%04x]\t\x1b[32mcmp\t\x1b[31mR%d, R%d\x1b[0m\n", vm->cpu.pc,
           vm->cpu.ir.rrr.sa, vm->cpu.ir.rrr.sb);
}

