#include <stdio.h>

#include "tracing.h"
#include "vm.h"

void dump_cpu(sigma16_cpu_t*);

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s [filename]\n", argv[0]);
        return -1;
    }

    char* fname = argv[1];
    sigma16_vm_t* vm = NULL;

    if (sigma16_vm_init(&vm, fname) < 0) {
        perror("failed to initialise vm");
        return -1;
    }
#ifdef ENABLE_TRACE
    vm->trace_handler = sigma16_trace;
    puts("==== INSTRUCTION TRACE ==== ");
#endif
    if (sigma16_vm_exec(vm) < 0) {
        perror("an error occured during execution");
        goto error;
    }
#ifdef ENABLE_TRACE
    dump_cpu(&vm->cpu);
#endif
#ifdef ENABLE_DUMP_MEM
    dump_vm_mem(vm);
#endif
    sigma16_vm_del(vm);
    return 0;
error:
    sigma16_vm_del(vm);
    return -1;
}

void dump_cpu(sigma16_cpu_t* cpu) {
    puts("==== CPU STATE ==== ");
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
