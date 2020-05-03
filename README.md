# Alternative Sigma16 Implementation

This is an alternative, CLI based, emulator for Sigma16. Sigma16 is a research architecture developed by [John T. O'Donnell](https://github.com/jtod) and used for the systems course at University of Glasgow. This project also includes Python bindings which allow for high performance while remaining accessible. The emulator can execute linearly without any interaction or drop the user in a debugger shell by modifying `config.h`.

### Further Development

While the core ISA has been implemented, the following have **not** been implemented:
- EXP instructions
- Debugger restart

### Performance

The emulator was able to outperform the [official emulator](https://jtod.github.io/home/Sigma16/) by 162,363 times (with tracing disabled). The official emulator took 3m 33.52s (+-1) whereas the alternative emulator took 2.4237e-3s (+-2.45%) to execute 12,951 instructions. From the previous results, it can be determined the emulator has a "clock", on my machine, of **~5.34MHz**. Further, the memory overhead of the emulator is capped at <6K (mostly VM memory).

## Installation

Installation and build instructions for C version (includes tracing):
```
$ git clone https://github.com/birb007/sigma16-emulator.git
$ cd sigma16-emulator
$ make
```

A `sigma16-emu` executable should then be present in the main repository directory. To use the emulator, specify an executable in the command line arguments.
```
usage: ./sigma16-emu [filename]
```

## Demonstration

An executable is a file consisting of machine code produced by the local assembler. A demonstration of the emulator usage using one of the included tests (written by John) is shown below.

![example usage](https://raw.githubusercontent.com/birb007/sigma16-emulator/master/assets/demo.png)

To better demonstrate the memory dump feature, `stackarray_simple.bin` was executed and the memory dump display is shown below.

![memory dump](https://raw.githubusercontent.com/birb007/sigma16-emulator/master/assets/mem_dump.png)

### Configuration

You can disable/cutomise various features by modifying `config.h`. Additionally, a user can modify `tracing.c` to include their own tracing functionality. If tracing is disabled the Python bindings will not expose a `trace_handler` kwarg to `sigma16.Emulator`. By default, the interactive debugger is enabled (this includes the Python bindings).

## Debugger

If the debugger is enabled the user will be dropped into an interactive environment prior to the execution of the first instruction. At this point the user can run any of the below commands.

```
Command          Description
-------          -----------
 ?             : display this message
 n (int)       : execute n steps
 i (int) (int) : write value to specified register
 o (int)       : display value of specified register
 t             : toggle tracing
 c             : continue execution
 d             : dump processor state
 m (int) ?(int): inspect memory from end to start
 b (int)       : set breakpoint at specified address
 e             : exit
```

However, the initial state of the processor/memory is undefined which affects the utility of commands prior to execution.

![debugger view](https://raw.githubusercontent.com/birb007/sigma16-emulator/master/assets/debugger.png)

## Python Bindings

Installation and build instructions for Python extension:
```
$ git clone https://github.com/birb007/sigma16-emulator.git
$ cd sigma16-emulator
$ python setup.py install
```

This will create a shared object which the Python interpreter can load as a module using regular `import` syntax.

To interact with the emulator we instantiate a `sigma16.Emulator` object and register a callback using the `trace_handler` kwarg. The callback will be called prior to every instruction executing within the emulator (if the emulator is compiled with `ENABLE_TRACE`), it is responsible for dispatching each instruction type to a different handler within Python.

### Example

Below an example application using the Python bindings for rudimentary tracing is shown.
```py
import sys
import sigma16


def rrr_handler(instruction: sigma16.InstructionRRR) -> None:
    print(
        f"RRR:\t[{instruction.op:02}] R{instruction.d}, "
        f"R{instruction.sa}, R{instruction.sb}"
    )


def rx_handler(instruction: sigma16.InstructionRX) -> None:
    print(
        f"RX:\t[{instruction.sb:02}] R{instruction.d}, "
        f"${instruction.disp:04x}[R{instruction.sa}]"
    )


def exp0_handler(instruction: sigma16.InstructionEXP0) -> None:
    print("EXP0:\t[{instruction.op:02}] R{instruction.d}, {instruction.sa}")


def handler(instruction) -> None:
    dispatch = {
        sigma16.InstructionRRR: rrr_handler,
        sigma16.InstructionRX: rx_handler,
        sigma16.InstructionEXP0: exp0_handler,
    }
    dispatch[type(instruction)](instruction)


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit(f"usage: {sys.argv[0]} [filename]")

    emu = sigma16.Emulator(sys.argv[1], trace_handler=handler)
    emu.execute()


if __name__ == "__main__":
    main()
```

Running the example using the fibonacci example we see:
```console
$ python assembler.py fib.s16 a.out
$ python example.py a.out
RX:	[00] R1, $0000[R0]
RX:	[00] R2, $0001[R0]
RX:	[00] R3, $000a[R0]
RX:	[00] R4, $0001[R0]
RRR:	[00] R5, R1, R2
RX:	[00] R1, $0000[R2]
RX:	[00] R2, $0000[R5]
RRR:	[01] R3, R3, R4
RX:	[00] R6, $007b[R0]
...
```

# Tooling

## Assembler

The assembler will assemble a specified source file into a binary to be ran under the emulator. The assembler uses the same mnemonics as the official emulator but there are a significant syntactic differences. Firstly, the assembler ignores all whitespace and is **case insensitive** (this includes label names).

### Grammar

```
labels      = ([a-Z]_) :
comments    = ; [string]
value       = 0x<hex> | 0b<binary> | 0<octal> | <decimal> | '<char>'
data        = data [value]
```

Comments may appear at the end of any line. However, labels must be placed on a separate line. Additionally, a new pseudo-instruction has been introduced as syntactic sugar: `mov`.

```
Opcode  Instruction     Description
f200    mov r16,r16     Mov r16 to r16.
f200    mov r16,imm16   Mov imm16 to r16.
```

It is encoded into an `lea` instruction. Further, the assembler can decode hexadecimal, octal, binary, decimal, and characters as values.

### Example

An example application is written below:
```armasm
start:
    mov r1, 0   ; fibonacci number
    mov r2, 1
    mov r3, 10  ; counter
    mov r4, 1
fib:
    add r5, r1, r2
    mov r1, r2
    mov r2, r5

    sub r3, r3, r4
    cmpeq r6, r3, r0
    jumpf r6, fib[r0]
exit:
    trap r0, r0, r0
```

We can compile this as follows:
```console
$ python assembler.py fib.s16 fib.bin
$ ./sigma16-emu fib.bin
[snip]
General Registers:
	R00: 0x00	0
	R01: 0x37	55
	R02: 0x59	89
	R03: 0x00	0
	R04: 0x01	1
	R05: 0x59	89
	R06: 0x01	1
...
```
We have successfully calculated the 10th fibonacci number, 55.

## Disassembler

The assembler will disassemble a specified binary file into a textual representation including the raw binary, file offset, Sigma16 memory offset, and the decoded mnemonics - if any. An example is shown below.

![disassembler output](https://raw.githubusercontent.com/birb007/sigma16-emulator/master/assets/disasm.png)

`[file offset|sigma16 offset] <raw hex> <mnemonic>`

The colours are intentional:

```
Colour  Object
------  ------
Yellow  opcode
Magenta destination
Blue    sa register
Green   sb register
Cyan    data
```

If the disassembler fails to decode instructions it will assume it is data. However, the disassembler will aggressively attempt to decode the entire file and display any valid instructions.
