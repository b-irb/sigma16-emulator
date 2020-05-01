import sys
import enum
import struct
import binascii

from dataclasses import dataclass
from typing import List, Dict, Union, TypeVar

OFF = "\x1b[0m"
BLACK = "\x1b[30m"
RED = "\x1b[31m"
GREEN = "\x1b[32m"
YELLOW = "\x1b[33m"
BLUE = "\x1b[34m"
MAGENTA = "\x1b[35m"
CYAN = "\x1b[36m"
WHITE = "\x1b[37m"


class DisassemblerError(Exception):
    def __init__(self, reason: str):
        super().__init__("error occured while disassembling")
        self.reason = reason


class Register(enum.IntEnum):
    r0 = 0
    r1 = 1
    r2 = 2
    r3 = 3
    r4 = 4
    r5 = 5
    r6 = 6
    r7 = 7
    r8 = 8
    r9 = 9
    r10 = 10
    r11 = 11
    r12 = 12
    r13 = 13
    r14 = 14
    r15 = 15


@dataclass
class Instruction:
    opcode: int
    mnemonic: str


@dataclass
class RRRInstruction(Instruction):
    d: Register
    sa: Register
    sb: Register

    def __str__(self):
        return (
            f"{BLUE}{self.mnemonic}\t{MAGENTA}{self.d.name:3} "
            f"{BLUE}{self.sa.name:3} {GREEN}{self.sb.name}{OFF}"
        )


@dataclass
class RXInstruction(Instruction):
    d: Register
    sa: Register
    sb: int
    disp: int

    def __str__(self):
        return (
            f"{YELLOW}{self.mnemonic}\t{MAGENTA}{self.d.name:3} "
            f"{OFF}{CYAN}0x{self.disp:02x}{OFF}[{BLUE}{self.sa.name}{OFF}]"
        )


RRR_INST_SIZE = 2
RX_INST_SIZE = 4


RRR_INSTRUCTIONS = [
    "add",
    "sub",
    "mul",
    "div",
    "cmp",
    "cmplt",
    "cmpeq",
    "cmpgt",
    "inv",
    "and",
    "or",
    "xor",
    "nop",
    "trap",
]
EXP_INSTRUCTIONS = []

RX_INSTRUCTIONS = [
    "lea",
    "load",
    "store",
    "jump",
    "jumpc0",
    "jumpc1",
    "jumpf",
    "jumpt",
    "jal",
]


def parse_register(r: int) -> Register:
    try:
        return Register(r)
    except ValueError:
        raise DisassemblerError("invalid register")


def split_byte(b: int):
    return b & 0xF, b >> 4


def decode_rrr_inst(binary: bytes, offset: int) -> RRRInstruction:
    d, opcode = split_byte(binary[offset])
    sb, sa = split_byte(binary[offset + 1])
    mnemonic = RRR_INSTRUCTIONS[opcode]
    return RRRInstruction(
        opcode, mnemonic, parse_register(d), parse_register(sa), parse_register(sb)
    )


def decode_rx_inst(binary: bytes, offset: int) -> RXInstruction:
    d, opcode = split_byte(binary[offset])
    sb, sa = split_byte(binary[offset + 1])
    try:
        mnemonic = RX_INSTRUCTIONS[sb]
    except IndexError:
        return None
    disp = struct.unpack(">H", binary[offset + 2 : offset + 4])[0]

    return RXInstruction(
        opcode,
        mnemonic,
        parse_register(d),
        parse_register(sa),
        parse_register(sb),
        disp,
    )


def disasm_binary(binary: bytes):
    len_binary = len(binary) - 1
    offset = 0

    while offset < len_binary:
        opcode = binary[offset] >> 4
        if 0 <= opcode < 15:
            instruction = decode_rrr_inst(binary, offset)
            offset += RRR_INST_SIZE
        elif opcode == 14:
            raise NotImplementedError("disassembler does not support EXP instructions")
        elif opcode == 15:
            instruction = decode_rx_inst(binary, offset)
            offset += RX_INST_SIZE
        else:
            raise DisassemblerError("invalid opcode")

        yield offset, instruction


def annotate_rrr_hex(instruction: RRRInstruction) -> str:
    return (
        f"{YELLOW}{instruction.opcode:x}{MAGENTA}{instruction.d:x} "
        f"{BLUE}{instruction.sa:x}{GREEN}{instruction.sb:x}{OFF}"
    )


def annotate_rx_hex(instruction: RXInstruction) -> str:
    return (
        f"{YELLOW}{instruction.opcode:x}{MAGENTA}{instruction.d:x} "
        f"{BLUE}{instruction.sa:x}{YELLOW}{instruction.sb:x} {CYAN}"
        + binascii.hexlify(instruction.disp.to_bytes(2, "big"), " ").decode()
        + OFF
    )


def annotate_inst_hex(instruction: Instruction) -> str:
    handler = {RRRInstruction: annotate_rrr_hex, RXInstruction: annotate_rx_hex}
    return handler[type(instruction)](instruction)


def main() -> None:
    if len(sys.argv) != 2:
        sys.exit(f"usage: {sys.argv[0]} in-file")

    try:
        with open(fname := sys.argv[1], "rb") as f:
            binary = f.read()
    except IOError:
        sys.exit(f"unable to read {fname}")

    prev_offset = 0
    try:
        for offset, instruction in disasm_binary(binary):
            fmt = f"[{prev_offset:04x}{RED}|{OFF}{prev_offset>>1:04x}] "
            if instruction is None:
                fmt += (
                    CYAN
                    + binascii.hexlify(binary[prev_offset:offset], " ").decode()
                    + OFF
                    + "\t ..."
                )
            else:
                hex_repr = annotate_inst_hex(instruction)
                fmt += f"{hex_repr}\t\t{instruction}"

            print(fmt)
            prev_offset = offset
    except DisassemblerError as e:
        sys.exit(f"[!!] error disassembling: {e.reason} at offset 0x{prev_offset:04x}")


if __name__ == "__main__":
    main()
