#pragma once
#include <stdint.h>

enum sigma16_instruction_fmt { RRR, RX, EXP };

typedef struct _sigma16_inst_rrr {
    uint8_t d : 4;
    uint8_t op : 4;
    uint8_t sb : 4;
    uint8_t sa : 4;
} sigma16_inst_rrr_t;

typedef struct _sigma16_inst_rx {
    uint8_t d : 4;
    uint8_t op : 4;
    uint8_t sb : 4;
    uint8_t sa : 4;
    uint16_t disp;
} sigma16_inst_rx_t;

typedef struct _sigma16_inst_exp0 {
    uint8_t d : 4;
    uint8_t op : 4;
    uint16_t ab;
} sigma16_inst_exp0_t;

typedef struct _sigma16_inst_exp4 {
    uint8_t d : 4;
    uint8_t op : 4;
    uint8_t ab : 4;
    uint8_t f : 4;
    uint8_t e : 4;
    uint8_t h : 4;
    uint8_t g : 4;
} sigma16_inst_exp4_t;

typedef struct _sigma16_inst_exp8 {
    uint8_t d : 4;
    uint8_t op : 4;
    uint8_t ab;
    uint8_t f : 4;
    uint8_t e : 4;
    uint8_t gh;
} sigma16_inst_exp8_t;
