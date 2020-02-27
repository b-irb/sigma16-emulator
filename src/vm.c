#include <stdio.h>
#include <byteswap.h>
#include <string.h>
#include "vm.h"
#include "cpu.h"
#include "instructions.h"

#define SAFE_UPDATE(src, val) \
    if (src != 0) \
        src = val;

#define APPLY_OP_RRR(vm, op) \
    INTERP_INST(vm, rrr); \
    if (vm->cpu.ir.rrr.d != 0) {\
        vm->cpu.regs[vm->cpu.ir.rrr.d] = vm->cpu.regs[vm->cpu.ir.rrr.sa] op vm->cpu.regs[vm->cpu.ir.rrr.sb]; \
        vm->cpu.pc += sizeof vm->cpu.ir.rrr; \
    }

#define INTERP_INST(vm, type) \
    vm->cpu.ir.type = *(sigma16_inst_##type##_t*)&((uint8_t*)vm->mem)[vm->cpu.pc]

#define INTERP_RX(vm) \
    INTERP_INST(vm, rx); \
    vm->cpu.ir.rx.disp = bswap_16(vm->cpu.ir.rx.disp)

#define SETFLAG(reg, flag, val) \
    (*(sigma16_reg_status_t*)&reg).flag = val

#define CLEARFLAGS(reg) \
    memset(&reg, 0, sizeof(sigma16_reg_status_t));


static uint16_t compute_rx_eaddr(sigma16_vm_t* vm) {
    vm->cpu.adr = vm->cpu.ir.rx.sa + vm->cpu.ir.rx.disp;
    return vm->cpu.adr;
}

int sigma16_vm_init(sigma16_vm_t** vm, char* fname) {
    FILE* executable;
    size_t exec_size;

    *vm = calloc(1, sizeof **vm);
    if (!*vm) {
        perror("unable to allocate memory for vm");
        return -1;
    }

    if (!(executable = fopen(fname, "rb"))) {
        perror("unable to open file");
        return -1;
    }

    fseek(executable, 0L, SEEK_END);
    exec_size = ftell(executable);
    fseek(executable, 0L, SEEK_SET);

    if (!((*vm)->mem = malloc(exec_size))) {
        perror("unable to allocate vm memory");
        goto error;
    }

    fread((*vm)->mem, exec_size, 1, executable);
    return 0;
error:
    free(vm);
    return -1;
}

void sigma16_vm_del(sigma16_vm_t* vm) {
    free(vm->mem);
    free(vm);
}

int sigma16_vm_exec(sigma16_vm_t* vm) {

    static const void* dispatch_table[] = {
        &&do_add, &&do_sub, &&do_mul, &&do_div, &&do_cmp, &&do_cmplt, &&do_cmpeq,
        &&do_cmpgt, &&do_invold, &&do_andold, &&do_orold, &&do_xorold, &&do_nop,
        &&do_trap, &&do_decode_exp, &&do_decode_rx
    };

    static const void* exp_dispatch_table[] = {
        &&do_rfi
    };

    static const void* rx_dispatch_table[] = {
        &&do_lea, &&do_load, &&do_store
    };

#define DISPATCH() goto *dispatch_table[((uint8_t*)vm->mem)[vm->cpu.pc]>>4]
    DISPATCH();

do_add:
    puts("add");
    APPLY_OP_RRR(vm, +);

    CLEARFLAGS(vm->cpu.regs[15]);
    SETFLAG(vm->cpu.regs[15], G, vm->cpu.ir.rrr.d > 0);
    SETFLAG(vm->cpu.regs[15], g, (int16_t)vm->cpu.ir.rrr.d > 0);
    SETFLAG(vm->cpu.regs[15], E, vm->cpu.ir.rrr.d == 0);
    SETFLAG(vm->cpu.regs[15], L, (int16_t)vm->cpu.ir.rrr.d < 0);
    SETFLAG(vm->cpu.regs[15], L, vm->cpu.ir.rrr.d == 0);
    // TODO overflow & carry
    SETFLAG(vm->cpu.regs[15], V, vm->cpu.ir.rrr.d > 0);
    SETFLAG(vm->cpu.regs[15], v, vm->cpu.ir.rrr.d > 0);
    SETFLAG(vm->cpu.regs[15], C, vm->cpu.ir.rrr.d > 0);

    DISPATCH();
do_sub:
    puts("sub");
    APPLY_OP_RRR(vm, -);
    DISPATCH();
do_mul:
    puts("mul");
    APPLY_OP_RRR(vm, *);
    DISPATCH();
do_div:
    puts("div");
    APPLY_OP_RRR(vm, /);
    DISPATCH();
do_cmp:
    puts("cmp");
    INTERP_INST(vm, rrr);

    CLEARFLAGS(vm->cpu.regs[15]);
    SETFLAG(vm->cpu.regs[15], G, vm->cpu.ir.rrr.sa > vm->cpu.ir.rrr.sb);
    SETFLAG(vm->cpu.regs[15], g, (int16_t)vm->cpu.ir.rrr.sa > (int16_t)vm->cpu.ir.rrr.sb);
    SETFLAG(vm->cpu.regs[15], E, vm->cpu.ir.rrr.sa == vm->cpu.ir.rrr.sb);
    SETFLAG(vm->cpu.regs[15], I, vm->cpu.ir.rrr.sa < vm->cpu.ir.rrr.sb);
    SETFLAG(vm->cpu.regs[15], L, (int16_t)vm->cpu.ir.rrr.sa < (int16_t)vm->cpu.ir.rrr.sb);

    DISPATCH();
do_cmplt:
    puts("cmplt");
    APPLY_OP_RRR(vm, <);

    CLEARFLAGS(vm->cpu.regs[15]);
    DISPATCH();
do_cmpeq:
    puts("cmpeq");
    APPLY_OP_RRR(vm, ==);

    CLEARFLAGS(vm->cpu.regs[15]);
    DISPATCH();
do_cmpgt:
    puts("cmpgt");
    APPLY_OP_RRR(vm, >);

    CLEARFLAGS(vm->cpu.regs[15]);
    DISPATCH();
do_invold:
    puts("invold");
    // APPLY_OP_RRR(vm, ~);

    CLEARFLAGS(vm->cpu.regs[15]);
    DISPATCH();
do_andold:
    puts("andold");
    APPLY_OP_RRR(vm, &);

    CLEARFLAGS(vm->cpu.regs[15]);
    DISPATCH();
do_orold:
    puts("orold");
    APPLY_OP_RRR(vm, |);

    CLEARFLAGS(vm->cpu.regs[15]);
    DISPATCH();
do_xorold:
    puts("xorold");
    APPLY_OP_RRR(vm, ^);

    CLEARFLAGS(vm->cpu.regs[15]);
    DISPATCH();
do_nop:
    puts("nop");
    vm->cpu.pc += sizeof vm->cpu.ir.rrr;

    CLEARFLAGS(vm->cpu.regs[15]);
    DISPATCH();
do_trap:
    puts("trap");
    goto end_hotloop;

    CLEARFLAGS(vm->cpu.regs[15]);
    DISPATCH();
do_decode_exp:
    puts("decode EXP");
    INTERP_INST(vm, exp0);
    goto *exp_dispatch_table[vm->cpu.ir.exp0.ab];
do_rfi:
    puts("rfi");
    // TODO rfi
    vm->cpu.pc += sizeof vm->cpu.ir.exp0;
    DISPATCH();
// TODO rest of exp instructions
do_decode_rx:
    puts("decode RX");
    INTERP_RX(vm);
    goto *rx_dispatch_table[vm->cpu.ir.rx.sb];
do_lea:
    puts("lea");
    vm->cpu.regs[vm->cpu.ir.rx.d] = compute_rx_eaddr(vm);
    vm->cpu.pc += sizeof vm->cpu.ir.rx;
    DISPATCH();
do_load:
    puts("load");
    vm->cpu.regs[vm->cpu.ir.rx.d] = vm->mem[compute_rx_eaddr(vm)];
    vm->cpu.pc += sizeof vm->cpu.ir.rx;
    DISPATCH();
do_store:
    puts("store");
    vm->mem[compute_rx_eaddr(vm)] = vm->cpu.regs[vm->cpu.ir.rx.d];
    vm->cpu.pc += sizeof vm->cpu.ir.rx;
    DISPATCH();
// TODO rest of rx instructions
do_jump:
    puts("jump");
    vm->cpu.pc = compute_rx_eaddr(vm);
    DISPATCH();
do_jumpf:
    puts("jumpf");
    vm->cpu.pc = (!vm->cpu.ir.rx.d ? compute_rx_eaddr(vm) : vm->cpu.pc + sizeof vm->cpu.ir.rx);
    DISPATCH();
do_jumpt:
    puts("jumpt");
    vm->cpu.pc = (vm->cpu.ir.rx.d ? compute_rx_eaddr(vm) : vm->cpu.pc + sizeof vm->cpu.ir.rx);
    DISPATCH();
end_hotloop:
    dump_regs(&vm->cpu);
    return 0;
error:
    return -1;
}
