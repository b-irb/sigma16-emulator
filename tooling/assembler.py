import sys
import enum
import struct

from dataclasses import dataclass
from typing import List, Dict, Union, TypeVar


class ParserError(Exception):
    def __init__(self, reason: str):
        super().__init__("error occured while parsing")
        self.reason = reason


class LinkerError(Exception):
    def __init__(self, reason: str):
        super().__init__("error occured while linking")
        self.reason = reason


class Register(enum.IntEnum):
    R0 = 0
    R1 = 1
    R2 = 2
    R3 = 3
    R4 = 4
    R5 = 5
    R6 = 6
    R7 = 7
    R8 = 8
    R9 = 9
    R10 = 10
    R11 = 11
    R12 = 12
    R13 = 13
    R14 = 14
    R15 = 15


@dataclass
class NumericConstant:
    value: int


@dataclass
class Instruction:
    opcode: int


@dataclass
class PseudoInstruction:
    alias: Instruction


@dataclass
class RRRInstruction(Instruction):
    d: Register
    sa: Register
    sb: Register


Identifier = str


@dataclass
class RXInstruction(Instruction):
    d: Register
    sa: Register
    disp: Union[int, Identifier]


@dataclass
class LabelBody:
    obj_offset: int
    code_offset: int


LabelMapping = Dict[Identifier, LabelBody]
Object = Union[Instruction, NumericConstant]


@dataclass
class Linker:
    objs: List[Object]
    labels: LabelMapping


INSTRUCTIONS = {
    # RRR instructions
    "ADD": (0, RRRInstruction),
    "SUB": (1, RRRInstruction),
    "MUL": (2, RRRInstruction),
    "DIV": (3, RRRInstruction),
    "CMP": (4, PseudoInstruction),
    "CMPLT": (5, RRRInstruction),
    "CMPEQ": (6, RRRInstruction),
    "CMPGT": (7, RRRInstruction),
    "INV": (8, PseudoInstruction),
    "AND": (9, RRRInstruction),
    "OR": (10, RRRInstruction),
    "XOR": (11, RRRInstruction),
    "NOP": (12, RRRInstruction),
    "TRAP": (13, RRRInstruction),
    # RX instructions
    "LEA": (0, RXInstruction),
    "LOAD": (1, RXInstruction),
    "STORE": (2, RXInstruction),
    "JUMP": (3, RXInstruction),
    "JUMPC0": (4, RXInstruction),
    "JUMPC1": (5, RXInstruction),
    "JUMPF": (6, RXInstruction),
    "JUMPT": (7, RXInstruction),
    # JX aliases
    "JUMPLE": (4, RXInstruction),
    "JUMPLT": (5, RXInstruction),
    "JUMPNE": (4, RXInstruction),
    "JUMPEQ": (5, RXInstruction),
    "JUMPGE": (4, RXInstruction),
    "JUMPGT": (5, RXInstruction),
    "JAL": (8, RXInstruction),
    # Custom instructions
    "MOV": (-1, PseudoInstruction),
}


def parse_numeric(string: str) -> int:
    SUPPORTED_BASES = [10, 16, 2, 8]

    for base in SUPPORTED_BASES:
        try:
            return int(string, base)
        except ValueError:
            pass

    if len(string) == 3 and string[0] == string[2] == "'":
        return ord(string[1])

    raise ParserError("invalid numeric base")


def parse_register(reg: str) -> Register:
    try:
        return Register[reg]
    except KeyError:
        raise ParserError(f'invalid register: "{reg}"')


def _parse_bin_inst(opcode: int, string: str) -> RRRInstruction:
    operand_chunks = string.split(",")
    operands = [
        parse_register(operand) for operand in map(lambda c: c.strip(), operand_chunks)
    ]
    return RRRInstruction(opcode, *operands, sb=Register["R0"])


def _parse_mov_inst(opcode: int, string: str) -> RXInstruction:
    def parse_reg_or_numeric(string: str):
        try:
            return parse_register(string)
        except ParserError:
            return parse_numeric(string)

    operand_chunks = string.split(",")
    d, val = [
        parse_reg_or_numeric(operand)
        for operand in map(lambda c: c.strip(), operand_chunks)
    ]
    if isinstance(val, Register):
        return RXInstruction(INSTRUCTIONS["LEA"][0], d=d, sa=val, disp=0)
    return RXInstruction(INSTRUCTIONS["LEA"][0], d=d, sa=Register["R0"], disp=val)


def _parse_pseudo_instruction(opcode: int, string: str) -> Instruction:
    handlers = {
        INSTRUCTIONS["CMP"][0]: _parse_bin_inst,
        INSTRUCTIONS["INV"][0]: _parse_bin_inst,
        INSTRUCTIONS["MOV"][0]: _parse_mov_inst,
    }

    return handlers[opcode](opcode, string)

    raise ParserError("invalid pseudo instruction")


def _parse_rrr_instruction(opcode: int, string: str) -> RRRInstruction:
    operand_chunks = string.split(",")
    operands = [
        parse_register(operand) for operand in map(lambda c: c.strip(), operand_chunks)
    ]

    if opcode == INSTRUCTIONS["CMP"][0]:
        return RRRInstruction(opcode, d=Register["R0"], *operands)

    return RRRInstruction(opcode, *operands)


def _parse_rx_instruction(opcode: int, string: str) -> RXInstruction:
    operand_chunks = string.split(",")
    implicit_dest = {
        INSTRUCTIONS["JUMP"][0]: Register(0),
        INSTRUCTIONS["JUMPLE"][0]: Register(1),
        INSTRUCTIONS["JUMPLT"][0]: Register(3),
        INSTRUCTIONS["JUMPNE"][0]: Register(2),
        INSTRUCTIONS["JUMPEQ"][0]: Register(2),
        INSTRUCTIONS["JUMPGE"][0]: Register(3),
        INSTRUCTIONS["JUMPGT"][0]: Register(1),
    }

    if opcode in implicit_dest:
        d = implicit_dest[opcode]
        disp, sa = operand_chunks[0].split("[")

    # jumpc0 and jumpc1 specify a bit in r15 as an integer constant
    # we treat this the same as a register to reduce code.
    # note: this _will_ produce register errors for invalid bit positions.
    else:
        d = parse_register(operand_chunks[0].strip())
        disp, sa = "".join(operand_chunks[1:]).split("[")

    try:
        disp = parse_numeric(disp.strip())
    except ParserError:
        if not is_legal_label_ref(disp.strip()):
            raise ParserError("invalid displacement")
        disp = Identifier(disp)

    sa = parse_register(sa.split("]")[0].strip())
    return RXInstruction(opcode, d, sa, disp)


def parse_instruction(string: str) -> Instruction:
    handlers = {
        PseudoInstruction: _parse_pseudo_instruction,
        RRRInstruction: _parse_rrr_instruction,
        RXInstruction: _parse_rx_instruction,
    }

    chunks = string.split()
    try:
        opcode, type_ = INSTRUCTIONS[chunks[0]]
    except KeyError:
        raise ParserError("unknown instruction name")

    return handlers[type_](opcode, "".join(chunks[1:]))


def is_legal_label_ref(string: str) -> bool:
    VALID_CHARS = set("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_")
    return set(string) | VALID_CHARS == VALID_CHARS


def parse_label(string: str) -> Identifier:
    if len(string) < 2 or string.index(":") != len(string) - 1:
        raise ParserError("invalid label")

    label = string[:-1]
    if is_legal_label_ref(label):
        return label

    raise ParserError("illegal char in label")


def parse_data_kw(string: str) -> NumericConstant:
    chunks = string.split()
    if chunks[0].strip() != "DATA":
        raise ParserError("missing data keyword")
    try:
        value = parse_numeric(chunks[1].strip())
    except ValueError:
        raise ParserError("invalid value")
    return NumericConstant(value)


def parse_string(string: str) -> Union[Object, Identifier]:
    chunks = string.strip().split()
    if chunks[0] == "DATA":
        return parse_data_kw(string)
    if chunks[0] in INSTRUCTIONS:
        return parse_instruction(string)
    return parse_label(string)


def parse_lines(lines: List[str]) -> Linker:
    linker = Linker(objs=[], labels={})
    errors = False
    parsed_objects = 0

    def verify(line: str) -> str:
        clean = line.strip()
        if clean == "":
            return ""
        return clean.split(";")[0]

    for idx, line in enumerate(lines):
        if not (content := verify(line)):
            continue

        try:
            obj = parse_string(content.upper())
        except ParserError as e:
            print(f"Parsing error: {e.reason}\nL{idx+1}\t{line.strip()}\n")
            errors = True
            continue

        if isinstance(obj, (Instruction, NumericConstant)):
            linker.objs.append(obj)
            parsed_objects += 1
        elif isinstance(obj, Identifier):
            if obj in linker.labels:
                raise ParserError(f'duplicate label "{obj}"')
            linker.labels[obj] = LabelBody(parsed_objects, 0)

    if errors:
        print("errors occured while parsing, aborting...")
    return linker


def sizeof(obj: Object) -> int:
    if isinstance(obj, RRRInstruction):
        return 1
    elif isinstance(obj, RXInstruction):
        return 2
    elif isinstance(obj, NumericConstant):
        return 1
    raise TypeError("unknown object type")


def calc_label_offset(linker: Linker) -> None:
    prev_off = offset = 0
    for label in linker.labels.values():
        for obj in linker.objs[prev_off : label.obj_offset]:
            offset += sizeof(obj)
        prev_off = label.obj_offset
        label.code_offset = offset


def link(linker: Linker) -> None:
    def validate(obj: Object) -> bool:
        return isinstance(obj, RXInstruction) and isinstance(obj.disp, Identifier)

    calc_label_offset(linker)

    for obj in filter(validate, linker.objs):
        try:
            obj.disp = linker.labels[obj.disp].code_offset
        except KeyError:
            raise LinkerError(f'unknown label "{obj.disp}"')


def write_code(handle, objs: List[Object]) -> None:
    for obj in objs:
        if isinstance(obj, RRRInstruction):
            head = (obj.opcode << 4) | obj.d.value
            tail = (obj.sa.value << 4) | obj.sb.value
            handle.write(struct.pack(">BB", head, tail))
        elif isinstance(obj, RXInstruction):
            head = (15 << 4) | obj.d.value
            body = (obj.sa.value << 4) | obj.opcode
            handle.write(struct.pack(">BBh", head, body, obj.disp))
        elif isinstance(obj, NumericConstant):
            handle.write(struct.pack(">h", obj.value))


def main() -> None:
    if len(sys.argv) != 3:
        sys.exit(f"usage: {sys.argv[0]} in-file out-file")

    ifile, ofile = sys.argv[1:]

    try:
        with open(fname := sys.argv[1]) as f:
            lines = f.readlines()
    except IOError:
        sys.exit(f"unable to read {fname}")

    linker = parse_lines(lines)
    link(linker)
    with open(ofile, "wb") as f:
        write_code(f, linker.objs)


if __name__ == "__main__":
    main()
