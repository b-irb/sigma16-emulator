#include "tracing.h"

#include <stdio.h>

#ifdef ENABLE_DEBUGGER
#include "debugger.h"
#endif
#include "cpu.h"
#include "events.h"
#include "instructions.h"
#include "vm.h"

const static char* RRR_INST_MNEMONICS[] = {
    "add",   "sub", "mul", "div", "cmp", "cmplt", "cmpeq",
    "cmpgt", "inv", "and", "or",  "xor", "nop",   "trap"};

const static char* RX_INST_MNEMONICS[] = {"lea",   "load",   "store",
                                          "jump",  "jumpc0", "jumpc1",
                                          "jumpf", "jumpt",  "jal"};

const static char* EXP_INST_MNEMONICS[] = {"rfi"};

void dump_cpu(sigma16_cpu_t*);
static void trace_rx(sigma16_vm_t*, const char*);
static void trace_rrr(sigma16_vm_t*, const char*);
static void trace_branch(sigma16_vm_t*, const char*);
static void trace_pseudoinst(sigma16_vm_t*, const char*);
static void print_status(sigma16_reg_status_t);

static void print_reg(sigma16_reg_t r) {
    switch (r) {
        case 0:
            printf(ANSI_GREEN "R0" ANSI_OFF);
            break;
        case 15:
            printf(ANSI_RED "R15" ANSI_OFF);
            break;
        default:
            printf(ANSI_MAGENTA "R%d" ANSI_OFF, r);
    }
}

static void print_value(uint16_t val) {
    if (!val) {
        printf(ANSI_WHITE "%04x" ANSI_OFF, val);
    } else {
        printf(ANSI_CYAN "%04x" ANSI_OFF, val);
    }
}

void sigma16_trace(sigma16_vm_t* vm, enum sigma16_trace_event event) {
    if (event == EXEC_START || event == EXEC_END) {
        return;
    }

    printf(ANSI_OFF "[%04x]\t", vm->cpu.pc);

    switch (event) {
        case INST_RRR:
            trace_rrr(vm, RRR_INST_MNEMONICS[vm->cpu.ir.rrr.op]);
            break;
        case INST_RX:
            trace_rx(vm, RX_INST_MNEMONICS[vm->cpu.ir.rx.sb]);
            break;
        case INST_EXP0:
            /* TODO EXP */
            break;
    }
}

static void trace_rx(sigma16_vm_t* vm, const char* mnemonic) {
    if (3 <= vm->cpu.ir.rx.sb && vm->cpu.ir.rx.sb <= 8) {
        trace_branch(vm, mnemonic);
    } else {
        printf(ANSI_YELLOW "%-05s\t", mnemonic);
        print_reg(vm->cpu.ir.rx.d);
        printf(", ");
        print_value(vm->cpu.ir.rx.disp);
        printf(ANSI_OFF "[");
        print_reg(vm->cpu.ir.rx.sa);
        puts(ANSI_OFF "]");
    }
}

static void trace_rrr(sigma16_vm_t* vm, const char* mnemonic) {
    if (vm->cpu.ir.rrr.op == 4 || vm->cpu.ir.rrr.op == 8) {
        trace_pseudoinst(vm, mnemonic);
    } else {
        printf(ANSI_BLUE "%-05s\t", mnemonic);
        print_reg(vm->cpu.ir.rrr.d);
        printf(", ");
        print_reg(vm->cpu.ir.rrr.sa);
        printf(", ");
        print_reg(vm->cpu.ir.rrr.sb);
        printf("\n");
    }
}

static void trace_branch(sigma16_vm_t* vm, const char* mnemonic) {
    printf(ANSI_RED "%-05s\t", mnemonic);

    /* jump instruction has useless d operand */
    if (vm->cpu.ir.rx.sb != 3) {
        print_reg(vm->cpu.ir.rx.d);
        printf(", ");
    }

    print_value(vm->cpu.ir.rx.disp);
    printf(ANSI_OFF "[");
    print_reg(vm->cpu.ir.rx.sa);
    puts(ANSI_OFF "]");
}

static void trace_pseudoinst(sigma16_vm_t* vm, const char* mnemonic) {
    printf(ANSI_BLUE "%-05s\t", mnemonic);
    print_reg(vm->cpu.ir.rrr.sa);
    printf(", ");
    print_reg(vm->cpu.ir.rrr.sb);
    printf("\n" ANSI_OFF);
}

static void print_status(sigma16_reg_status_t stat) {
    if (stat.C) {
        printf(ANSI_YELLOW "C");
    } else {
        printf(ANSI_BLUE "C");
    }

    if (stat.v) {
        printf(ANSI_YELLOW "v");
    } else {
        printf(ANSI_BLUE "v");
    }

    if (stat.V) {
        printf(ANSI_YELLOW "V");
    } else {
        printf(ANSI_BLUE "V");
    }

    if (stat.L) {
        printf(ANSI_YELLOW "L");
    } else {
        printf(ANSI_BLUE "L");
    }

    if (stat.l) {
        printf(ANSI_YELLOW "l");
    } else {
        printf(ANSI_BLUE "l");
    }

    if (stat.E) {
        printf(ANSI_YELLOW "E");
    } else {
        printf(ANSI_BLUE "E");
    }

    if (stat.g) {
        printf(ANSI_YELLOW "g");
    } else {
        printf(ANSI_BLUE "g");
    }

    if (stat.G) {
        printf(ANSI_YELLOW "G" ANSI_OFF);
    } else {
        printf(ANSI_BLUE "G" ANSI_OFF);
    }
}

void dump_cpu(sigma16_cpu_t* cpu) {
    puts("General Registers:");
    for (int i = 0; i < 15; ++i) {
        printf(ANSI_MAGENTA "R%02d: " ANSI_OFF, i);
        print_value(cpu->regs[i]);
        if (cpu->regs[i]) {
            printf(ANSI_CYAN "\t%d\n" ANSI_OFF, cpu->regs[i]);
        } else {
            puts(ANSI_WHITE "\t0" ANSI_OFF);
        }
    }

    printf(ANSI_MAGENTA "R15: " ANSI_OFF);
    print_value(cpu->regs[15]);
    printf("\t");
    print_status(*((sigma16_reg_status_t*)&cpu->regs[15]));

    puts("\nControl Registers:");
    puts(ANSI_RED "IR:\t" ANSI_BLUE "N/A" ANSI_OFF);

    printf(ANSI_RED "PC:\t");
    print_value(cpu->pc);

    printf("\n" ANSI_RED "ADR:\t");
    print_value(cpu->adr);

    printf("\n" ANSI_RED "DAT:\t");
    print_value(cpu->dat);

    printf("\n" ANSI_RED "STATUS:\t");
    print_value(*((sigma16_reg_t*)&cpu->status));
    printf("\t");
    print_status(cpu->status);

    puts(ANSI_OFF "\nStatus Register Flags:");
    printf(ANSI_RED "SYS:\t");
    print_value(cpu->sys);

    printf("\n" ANSI_RED "IE:\t");
    print_value(cpu->ie);

    puts(ANSI_OFF "\nInterrupt and Exceptions:");
    printf(ANSI_RED "MASK:\t");
    print_value(cpu->mask);

    printf("\n" ANSI_RED "REQ:\t");
    print_value(cpu->req);

    printf("\n" ANSI_RED "ISTAT:\t");
    print_value(*((sigma16_reg_t*)&cpu->istat));
    printf("\t");
    print_status(cpu->istat);

    printf("\n" ANSI_RED "IPC:\t");
    print_value(cpu->ipc);

    printf("\n" ANSI_RED "VECT:\t");
    print_value(cpu->vect);
    printf("\n");
}

static void print_array_char(char* buf, int size) {
    char printable;

    for (int j = 0; j < size; ++j) {
        printable = buf[j];
        if (printable != '.') {
            printf(ANSI_YELLOW "%c" ANSI_OFF, printable);
        } else {
            printf(ANSI_WHITE "%c" ANSI_OFF, printable);
        }
    }
    printf("\n");
}

void dump_vm_mem(sigma16_vm_t* vm, size_t start, size_t end) {
    int i;
    uint16_t word;
    unsigned char printable;
    unsigned char buf[17] = {};

    for (i = start; i < end; ++i) {
        word = read_mem(vm, i);

        if ((i % 16) == 0) {
            if (i != 0) {
                print_array_char(buf, 17);
            }
            printf("[%04x] ", i);
        }

        print_value(word);
        printf(" ");

        printable = word & 0xff;
        if (printable < ' ' || printable > '~') {
            buf[i % 16] = '.';
        } else {
            buf[i % 16] = printable;
        }
    }

    while (i++ % 16 != 0) {
        printf("     ");
    }

    print_array_char(buf, 17);
}

