#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "tracing.h"
#include "vm.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* fname = argv[1];
    sigma16_vm_t* vm = NULL;

    if (sigma16_vm_init(&vm, fname) < 0) {
        perror("failed to initialise vm");
        return EXIT_FAILURE;
    }
#ifdef ENABLE_TRACE
    vm->trace_handler = sigma16_trace;
    puts("Instruction Trace:");
#endif
    if (sigma16_vm_exec(vm) < 0) {
        perror("an error occured during execution");
        goto error;
    }
#ifdef ENABLE_CPU_DUMP
    puts("Termination.\n");
    dump_cpu(&vm->cpu);
#endif
#ifdef ENABLE_DUMP_MEM
    puts("Memory:");
    dump_vm_mem(vm);
#endif
    sigma16_vm_del(vm);
    return 0;
error:
    sigma16_vm_del(vm);
    return EXIT_FAILURE;
}

