#include "cpu.h"
#include <stdio.h>

void dump_regs(sigma16_cpu_t* cpu) {
    puts("Regs:");
    // general purpose regs
    for (int i=0; i<16; ++i) {
        printf("\t R%02d: 0x%x\t%u\n", i, (unsigned int)cpu->regs[i], (unsigned int)cpu->regs[i]);
    }
}
