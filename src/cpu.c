#include "cpu.h"
#include <stdio.h>

void dump_cpu(sigma16_cpu_t* cpu) {
    puts("============= CPU STATE ============= ");
    puts("General Registers:");
    // general purpose regs
    for (int i=0; i<16; ++i) {
        printf("\t\x1b[31mR%02d:\x1b[0m ", i);

        if (cpu->regs[i]) {
            printf("\x1b[33m0x%02x\x1b[0m\t\x1b[32m%d\x1b[0m\n",
                    (unsigned int)cpu->regs[i],
                    (unsigned int)cpu->regs[i]);
        } else {
            printf("0x%02x\t%d\n",
                    (unsigned int)cpu->regs[i],
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
