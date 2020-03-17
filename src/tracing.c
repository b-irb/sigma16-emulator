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

void sigma16_trap_instruction(sigma16_vm_t* vm,
                              enum sigma16_instruction_fmt fmt) {
    switch (fmt) {
        case RRR:
            trace_rrr(vm, RRR_INST_MNEMONICS[vm->cpu.ir.rrr.op]);
            break;
        case RX:
            trace_rx(vm, RX_INST_MNEMONICS[vm->cpu.ir.rx.d]);
            break;
            /* TODO EXP*/
    }
}

void sigma16_trap_beg_execution(sigma16_vm_t* vm) {
    puts("===== INSTRUCTION TRACE ====");
}

void sigma16_trap_end_execution(sigma16_vm_t* vm) { dump_cpu(&vm->cpu); }

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

void dump_cpu(sigma16_cpu_t* cpu) {
    puts("============= CPU STATE ============= ");
    puts("General Registers:");
    for (int i = 0; i < 16; ++i) {
        printf("\t\x1b[31mR%02d:\x1b[0m ", i);

        if (cpu->regs[i]) {
            printf("\x1b[33m0x%02x\x1b[0m\t\x1b[32m%d\x1b[0m\n",
                   (unsigned int)cpu->regs[i], (unsigned int)cpu->regs[i]);
        } else {
            printf("0x%02x\t%d\n", (unsigned int)cpu->regs[i],
                   (unsigned int)cpu->regs[i]);
        }
    }

    puts("Control Registers:");
    printf("\t\x1b[31mIR:\t\x1b[34mN/A\x1b[0m\n");
    printf("\t\x1b[31mPC:\x1b[0m\t0x%08x\n", (unsigned int)cpu->pc);
    printf("\t\x1b[31mADR:\x1b[0m\t0x%08x\n", (unsigned int)cpu->adr);
    printf("\t\x1b[31mDAT:\x1b[0m\t0x%08x\n", (unsigned int)cpu->dat);
    // printf("\tSTATUS: 0x%08x\n", (unsigned int)cpu->status);

    puts("Status Register Flags");
    printf("\t\x1b[31mSYS:\x1b[0m\t");
    if (cpu->sys) {
        printf("\x1b[33m%d\x1b[0m\n", cpu->sys);
    } else {
        printf("%d\n", cpu->sys);
    }

    printf("\t\x1b[31mIE:\x1b[0m\t");
    if (cpu->ie) {
        printf("\x1b[33m%d\x1b[0m\n", cpu->ie);
    } else {
        printf("%d\n", cpu->ie);
    }

    puts("Interrupt and Exceptions");
    printf("\t\x1b[31mMASK:\x1b[0m\t");
    if (cpu->mask) {
        printf("\x1b[32m0x%08x\x1b[0m\n", (unsigned int)cpu->mask);
    } else {
        printf("0x%08x\n", (unsigned int)cpu->mask);
    }

    printf("\t\x1b[31mREQ:\x1b[0m\t0x%08x\n", (unsigned int)cpu->req);

    // printf("\tISTAT: 0x%08x\n", (unsigned int)cpu->istat);
    printf("\t\x1b[31mIPC:\x1b[0m\t0x%08x\n", (unsigned int)cpu->ipc);
    printf("\t\x1b[31mVECT:\x1b[0m\t0x%08x\n", (unsigned int)cpu->vect);
}
