#include <stdio.h>
#include "vm.h"

int main(int argc, char** argv)
{
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
    if (sigma16_vm_exec(vm) < 0) {
        perror("an error occured during execution");
        goto error;
    }
#ifdef DUMP_CPU
    dump_cpu(&vm->cpu);
#endif
#ifdef DUMP_MEM
    dump_vm_mem(vm);
#endif
    sigma16_vm_del(vm);
    return 0;
error:
    sigma16_vm_del(vm);
    return -1;
}
