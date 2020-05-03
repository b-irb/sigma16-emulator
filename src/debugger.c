#include "debugger.h"

#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tracing.h"
#include "vm.h"

#ifdef ENABLE_DEBUGGER
enum debugger_cmd_action { RESUME, PROMPT, ERROR };

static void debugger_teardown(struct debugger_ctx* ctx) {
    sigma16_vm_del(ctx->vm);
}

static int parse_int(char* buf, int def) {
    int val;

    if (!buf) {
        val = def;
    } else {
        val = atoi(buf);
    }
    return val;
}

static void destroy_cmd(struct debugger_cmd* cmd) {
    switch (cmd->cmd) {
        case RESTART:
            free(cmd->args->s);
            break;
    }

    if (cmd->args) {
        free(cmd->args);
    }
    free(cmd);
}

static struct debugger_cmd* create_cmd(void) {
    return malloc(sizeof(struct debugger_cmd));
}

static union debugger_arg* create_arg(int n) {
    return malloc(sizeof(union debugger_arg) * n);
}

static struct debugger_cmd* create_cmd_restart(char* fname) {
    struct debugger_cmd* cmd;
    union debugger_arg* arg;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    arg = create_arg(1);
    arg->s = strdup(fname);

    cmd->cmd = RESTART;
    cmd->args = arg;
    return cmd;
}

static struct debugger_cmd* create_cmd_step(int steps) {
    struct debugger_cmd* cmd;
    union debugger_arg* arg;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    arg = create_arg(1);
    arg->i = steps;

    cmd->cmd = STEP;
    cmd->args = arg;
    return cmd;
}

static struct debugger_cmd* create_cmd_continue(void) {
    struct debugger_cmd* cmd;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    cmd->cmd = CONTINUE;
    cmd->args = NULL;
    return cmd;
}

static struct debugger_cmd* create_cmd_help(void) {
    struct debugger_cmd* cmd;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    cmd->cmd = HELP;
    cmd->args = NULL;
    return cmd;
}

static struct debugger_cmd* create_cmd_dump_cpu(void) {
    struct debugger_cmd* cmd;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    cmd->cmd = DUMP_CPU;
    cmd->args = NULL;
    return cmd;
}

static struct debugger_cmd* create_cmd_write_reg(int reg, int val) {
    struct debugger_cmd* cmd;
    union debugger_arg* arg;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    arg = create_arg(2);
    arg[0].i = reg;
    arg[1].i = val;

    cmd->cmd = WRITE_REG;
    cmd->args = arg;
    return cmd;
}

static struct debugger_cmd* create_cmd_set_breakpoint(int addr) {
    struct debugger_cmd* cmd;
    union debugger_arg* arg;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    arg = create_arg(1);
    arg[0].i = addr;

    cmd->cmd = SET_BREAKPOINT;
    cmd->args = arg;
    return cmd;
}

static struct debugger_cmd* create_cmd_read_reg(int reg) {
    struct debugger_cmd* cmd;
    union debugger_arg* arg;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    arg = create_arg(2);
    arg[0].i = reg;

    cmd->cmd = READ_REG;
    cmd->args = arg;
    return cmd;
}

static struct debugger_cmd* create_cmd_dump_mem(int start, int end) {
    struct debugger_cmd* cmd;
    union debugger_arg* arg;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    arg = create_arg(2);
    arg[0].i = start;
    arg[1].i = end;

    cmd->cmd = DUMP_MEM;
    cmd->args = arg;
    return cmd;
}

static struct debugger_cmd* create_cmd_exit(void) {
    struct debugger_cmd* cmd;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    cmd->cmd = EXIT;
    cmd->args = NULL;
    return cmd;
}

static struct debugger_cmd* create_cmd_trace(void) {
    struct debugger_cmd* cmd;

    if (!(cmd = create_cmd())) {
        return NULL;
    }

    cmd->cmd = TRACE;
    cmd->args = NULL;
    return cmd;
}

static struct debugger_cmd* parse_cmd_write_reg(struct debugger_ctx* ctx,
                                                char* buf) {
    char* token;
    int reg;
    int val;

    if ((token = strtok(NULL, " "))) {
        reg = atoi(token);
    } else {
        fprintf(stderr, "specify register\n");
        return NULL;
    }
    if ((token = strtok(NULL, " "))) {
        val = atoi(token);
    } else {
        fprintf(stderr, "specify value\n");
        return NULL;
    }

    return create_cmd_write_reg(reg, val);
}

static struct debugger_cmd* parse_cmd_read_reg(struct debugger_ctx* ctx,
                                               char* buf) {
    char* token;
    int reg;

    if ((token = strtok(NULL, " "))) {
        reg = atoi(token);
    } else {
        fprintf(stderr, "specify register\n");
        return NULL;
    }

    return create_cmd_read_reg(reg);
}

static struct debugger_cmd* parse_cmd_step(struct debugger_ctx* ctx,
                                           char* buf) {
    char* token;
    int steps;

    token = strtok(NULL, " ");
    steps = parse_int(token, 1);

    return create_cmd_step(steps);
}

static struct debugger_cmd* parse_cmd_help(struct debugger_ctx* ctx,
                                           char* buf) {
    return create_cmd_help();
}

static struct debugger_cmd* parse_cmd_dump_cpu(struct debugger_ctx* ctx,
                                               char* buf) {
    return create_cmd_dump_cpu();
}

static struct debugger_cmd* parse_cmd_dump_mem(struct debugger_ctx* ctx,
                                               char* buf) {
    int end;
    int start;

    end = parse_int(strtok(NULL, " "), 0x200 >> 1);
    start = parse_int(strtok(NULL, " "), 0);
    return create_cmd_dump_mem(end, start);
}

static struct debugger_cmd* parse_cmd_set_breakpoint(struct debugger_ctx* ctx,
                                                     char* buf) {
    int addr;
    char* token = strtok(NULL, " ");

    if (!token) {
        fprintf(stderr, "invalid breakpoint\n");
    }
    addr = atoi(token);
    return create_cmd_set_breakpoint(addr);
}

static struct debugger_cmd* parse_cmd_restart(struct debugger_ctx* ctx,
                                              char* buf) {
    char* fname;
    fname = strtok(NULL, " ");
    if (!fname) {
        fname = ctx->source;
    }
    return create_cmd_restart(fname);
}

static struct debugger_cmd* parse_cmd_exit(struct debugger_ctx* ctx,
                                           char* buf) {
    return create_cmd_exit();
}

static struct debugger_cmd* parse_cmd_continue(struct debugger_ctx* ctx,
                                               char* buf) {
    return create_cmd_continue();
}

static struct debugger_cmd* parse_cmd_trace(struct debugger_ctx* ctx,
                                            char* buf) {
    return create_cmd_trace();
}

static struct debugger_cmd* parse_cmd(struct debugger_ctx* ctx) {
    char* token;
    char* buf;
    struct debugger_cmd* cmd = NULL;

    while ((buf = readline("(s16) "))) {
        if (strlen(buf)) {
            break;
        }
    }

    add_history(buf);
    token = strtok(buf, " ");

    if (!token) {
        free(buf);
        return NULL;
    }
    /*
     *if (!strcmp(token, "r")) {
     *    return parse_cmd_restart(ctx, buf);
     *}
     */
    if (!strcmp(token, "n")) {
        cmd = parse_cmd_step(ctx, buf);
    }
    if (!strcmp(token, "b")) {
        cmd = parse_cmd_set_breakpoint(ctx, buf);
    }
    if (!strcmp(token, "i")) {
        cmd = parse_cmd_write_reg(ctx, buf);
    }
    if (!strcmp(token, "o")) {
        cmd = parse_cmd_read_reg(ctx, buf);
    }
    if (!strcmp(token, "t")) {
        cmd = parse_cmd_trace(ctx, buf);
    }
    if (!strcmp(token, "?")) {
        cmd = parse_cmd_help(ctx, buf);
    }
    if (!strcmp(token, "c")) {
        cmd = parse_cmd_continue(ctx, buf);
    }
    if (!strcmp(token, "d")) {
        cmd = parse_cmd_dump_cpu(ctx, buf);
    }
    if (!strcmp(token, "m")) {
        cmd = parse_cmd_dump_mem(ctx, buf);
    }
    if (!strcmp(token, "e")) {
        cmd = parse_cmd_exit(ctx, buf);
    }

    free(buf);
    return cmd;
}

static enum debugger_cmd_action debugger_dump_mem(struct debugger_ctx* ctx,
                                                  struct debugger_cmd* cmd) {
    dump_vm_mem(ctx->vm, cmd->args[1].i, cmd->args[0].i);
    return PROMPT;
}

static enum debugger_cmd_action debugger_dump_cpu(struct debugger_ctx* ctx,
                                                  struct debugger_cmd* cmd) {
    dump_cpu(&ctx->vm->cpu);
    return PROMPT;
}

static enum debugger_cmd_action debugger_help(struct debugger_ctx* ctx,
                                              struct debugger_cmd* cmd) {
    puts(
        "Command        : Description\n"
        "-------        : -----------\n"
        " ?             : display this message\n"
        " n (int)       : execute n steps\n"
        " i (int) (int) : write value to specified register\n"
        " o (int)       : display value of specified register\n"
        " t             : toggle tracing\n"
        " c             : continue execution\n"
        " d             : dump processor state\n"
        " m (int) ?(int): inspect memory from end to start\n"
        " b (int)       : set breakpoint at specified address\n"
        " e             : exit");
    return PROMPT;
}

/*
 *static enum debugger_cmd_action debugger_restart(struct debugger_ctx* ctx,
 *                                                 struct debugger_cmd* cmd) {
 *    char* fname = cmd->args->s;
 *
 *    puts("debugger restart");
 *    sigma16_vm_del(ctx->vm);
 *
 *    if (sigma16_vm_init(&(ctx->vm), fname) < 0) {
 *        fprintf(stderr, "unable to re-init VM\n");
 *        return -1;
 *    }
 *
 *    ctx->source = fname;
 *    printf("fname: %s\n", ctx->source);
 *
 *    return RESUME;
 *}
 */

static enum debugger_cmd_action debugger_write_reg(struct debugger_ctx* ctx,
                                                   struct debugger_cmd* cmd) {
    int idx = cmd->args[0].i;

    if (0 > idx && idx < 15) {
        fprintf(stderr, "invalid register\n");
        return PROMPT;
    }

    ctx->vm->cpu.regs[idx] = cmd->args[1].i;
    return PROMPT;
}

static enum debugger_cmd_action debugger_set_breakpoint(
    struct debugger_ctx* ctx, struct debugger_cmd* cmd) {
    static int id;
    struct debugger_bp* bp;

    if (!(bp = malloc(sizeof(*bp)))) {
        fprintf(stderr, "unable to create breakpoint\n");
        return ERROR;
    }

    bp->id = id++;
    bp->addr = cmd->args[0].i;
    bp->next = ctx->breakpoints;
    ctx->breakpoints = bp;

    return PROMPT;
}

static enum debugger_cmd_action debugger_read_reg(struct debugger_ctx* ctx,
                                                  struct debugger_cmd* cmd) {
    int val;
    int idx = cmd->args[0].i;

    if (0 > idx && idx < 15) {
        fprintf(stderr, "invalid register\n");
        return PROMPT;
    }

    val = ctx->vm->cpu.regs[idx];
    printf("R%d=%d (0x%04x)\n", idx, val, val);
    return PROMPT;
}

static enum debugger_cmd_action debugger_step(struct debugger_ctx* ctx,
                                              struct debugger_cmd* cmd) {
    ctx->n_steps = cmd->args->i;
    return RESUME;
}

static enum debugger_cmd_action debugger_continue(struct debugger_ctx* ctx,
                                                  struct debugger_cmd* cmd) {
    puts("Continuing.");
    return RESUME;
}

static enum debugger_cmd_action debugger_trace(struct debugger_ctx* ctx,
                                               struct debugger_cmd* cmd) {
    ctx->trace ^= 1;
    printf("tracing=%d\n", ctx->trace);
    return PROMPT;
}

static void debugger_exit(struct debugger_ctx* ctx, struct debugger_cmd* cmd) {
    destroy_cmd(cmd);
    puts("Quit.");
    exit(0);
}

static void debugger_interactive(struct debugger_ctx* ctx) {
    enum debugger_cmd_action action = RESUME;
    struct debugger_cmd* cmd;

    while (1) {
        if (!(cmd = parse_cmd(ctx))) {
            fprintf(stderr, "invalid command\n");
            continue;
        }

        switch (cmd->cmd) {
            case EXIT:
                debugger_exit(ctx, cmd);
                break;
            case STEP:
                action = debugger_step(ctx, cmd);
                break;
            case TRACE:
                action = debugger_trace(ctx, cmd);
                break;
            case READ_REG:
                action = debugger_read_reg(ctx, cmd);
                break;
            case WRITE_REG:
                action = debugger_write_reg(ctx, cmd);
                break;
            case SET_BREAKPOINT:
                action = debugger_set_breakpoint(ctx, cmd);
                break;
            /*
             *case RESTART:
             *    action = debugger_restart(ctx, cmd);
             *    break;
             */
            case HELP:
                action = debugger_help(ctx, cmd);
                break;
            case CONTINUE:
                action = debugger_continue(ctx, cmd);
                break;
            case DUMP_CPU:
                action = debugger_dump_cpu(ctx, cmd);
                break;
            case DUMP_MEM:
                action = debugger_dump_mem(ctx, cmd);
                break;
        }

        destroy_cmd(cmd);

        switch (action) {
            case RESUME:
                return;
            case PROMPT:
                continue;
            case ERROR:
                goto error;
        }
    }
error:
    fprintf(stderr, "an error occured, tearing down debugger\n");
    debugger_teardown(ctx);
    abort();
}

void yield_debugger(sigma16_vm_t* vm, enum sigma16_trace_event event) {
    struct debugger_ctx* ctx = vm->debugger;

    ctx->n_events++;

    if (ctx->trace) {
        sigma16_trace(vm, event);
    }

    if (!--ctx->n_steps) {
        debugger_interactive(ctx);
    }

    if (event == EXEC_START) {
        debugger_interactive(vm->debugger);
    } else if (event == EXEC_END) {
        puts("Post execution (limited commands).");
        debugger_interactive(vm->debugger);
    }

    for (struct debugger_bp* bp = ctx->breakpoints; bp; bp = bp->next) {
        if (vm->cpu.pc == bp->addr) {
            printf("breakpoint %d hit\n", bp->id);
            debugger_interactive(ctx);
            break;
        }
    }
}

sigma16_vm_t* debugger_init(char* fname) {
    sigma16_vm_t* vm;
    struct debugger_ctx* ctx;

    if (!(ctx = malloc(sizeof *ctx))) {
        return NULL;
    }

    vm = ctx->vm;

    if (sigma16_vm_init(&vm, fname) < 0) {
        fprintf(stderr, "unable to initialise VM\n");
        goto error;
    }

    ctx->source = fname;
    ctx->n_steps = -1;
    ctx->breakpoints = NULL;
    ctx->vm = vm;
    ctx->trace = 1;

    vm->debugger = ctx;
    vm->trace_handler = yield_debugger;
    return vm;
error:
    debugger_teardown(ctx);
    return NULL;
}
#endif
