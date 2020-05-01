#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#ifdef ENABLE_DEBUGGER
#include "debugger.h"
#endif
#include "tracing.h"
#include "vm.h"

#ifdef ENABLE_DEBUGGER
int exec_debugger(char* fname) {
    sigma16_vm_t* vm;

    if (!(vm = debugger_init(fname))) {
        fprintf(stderr, "unable to initialise debugger\n");
        return EXIT_FAILURE;
    }

    if (sigma16_vm_exec(vm) < 0) {
        perror("an error occured during execution");
        return EXIT_FAILURE;
    }
    return 0;
}
#endif

int exec_normal(char* fname) {
    sigma16_vm_t* vm;

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
    dump_vm_mem(vm, 0, DUMP_MEM_LIM);
#endif
    sigma16_vm_del(vm);
    return 0;
error:
    sigma16_vm_del(vm);
    return EXIT_FAILURE;
}

int main(int argc, char** argv) {
    sigma16_vm_t* vm = NULL;
    char* fname;

    if (argc < 2) {
        fprintf(stderr, "usage: %s [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    fname = argv[1];

    return
#ifndef ENABLE_DEBUGGER
        exec_normal(fname);
#else
        exec_debugger(fname);
#endif
}

