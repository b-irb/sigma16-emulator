#pragma once
#include <stdint.h>
#include "instructions.h"

#define REGSTATUS(vm) (*((sigma16_reg_status_t*)&vm->cpu.regs[15]))

typedef uint16_t sigma16_reg_t;

union sigma16_inst_variant {
    sigma16_inst_rrr_t rrr;
    sigma16_inst_rx_t rx;
    sigma16_inst_exp0_t exp0;
    sigma16_inst_exp4_t exp4;
    sigma16_inst_exp8_t exp8;
};

typedef struct _sigma16_mask_flags {
    uint8_t trap: 1;
    uint8_t overflow: 1;
    uint8_t div0: 1;
    uint8_t stackfault: 1;
    uint8_t segfault: 1;
    uint8_t privelege: 1;
    uint8_t timer: 1;
    uint8_t input: 1;
    uint8_t output: 1;
} sigma16_mask_flags;

typedef struct _sigma16_reg_status {
    uint8_t _padding;
    uint8_t C: 1;
    uint8_t v: 1;
    uint8_t V: 1;
    uint8_t L: 1;
    uint8_t l: 1;
    uint8_t E: 1;
    uint8_t g: 1;
    uint8_t G: 1;
} sigma16_reg_status_t;

typedef struct _sigma16_cpu {
    sigma16_reg_t regs[16];
    union sigma16_inst_variant ir;
    sigma16_reg_t pc;
    sigma16_reg_t adr;
    sigma16_reg_t dat;
    sigma16_reg_status_t status;
    _Bool sys;
    _Bool ie;
    sigma16_reg_t mask;
    sigma16_reg_t req;
    sigma16_reg_status_t istat;
    sigma16_reg_t ipc;
    sigma16_reg_t vect;
} sigma16_cpu_t;
