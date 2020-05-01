#pragma once
#include "vm.h"

struct debugger_bp {
    int id;
    uint16_t addr;
    struct debugger_bp* next;
};

struct debugger_ctx {
    int n_steps;
    int n_events;
    _Bool trace;
    char* source;
    sigma16_vm_t* vm;
    struct debugger_bp* breakpoints;
};

enum debugger_cmd_type {
    RESTART,
    STEP,
    CONTINUE,
    TRACE,
    DUMP_CPU,
    DUMP_MEM,
    WRITE_REG,
    READ_REG,
    HELP,
    EXIT
};

union debugger_arg {
    int i;
    char* s;
};

struct debugger_cmd {
    enum debugger_cmd_type cmd;
    union debugger_arg* args;
};

sigma16_vm_t* debugger_init(char*);
