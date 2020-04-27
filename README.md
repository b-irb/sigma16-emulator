# Alternative Sigma16 Implementation

This is an alternative, CLI based, emulator for Sigma16. Sigma16 is a research architecture developed by [John T. O'Donnell](https://github.com/jtod) and used for the systems course at University of Glasgow. This project also includes Python bindings which allow for high performance and greater accessibility.

While the core ISA has been implemented, the following have **not** been implemented:
- EXP instructions
- The `inv` instruciton

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

Installation and build instructions for Python extension:
```
$ git clone https://github.com/birb007/sigma16-emulator.git
$ cd sigma16-emulator
$ python setup.py install
```

This will create a shared object which the Python interpreter can load as a module using regular `import` syntax.

Example application using the Python bindings for rudimentary tracing is shown below.
```py
import sigma16

def rrr_handler(instruction: InstructionRRR) -> None:
    print(f"RRR:\t(op) {instruction.op}")

def rx_handler(instruction: InstructionRX) -> None:
    print("RX:\t(op) {instruction.d}")

def exp0_handler(instruction: InstructionEXP0) -> None:
    print("EXP0:\t(op) {instruction.sa}")

def handler(instruction) -> None:
    dispatch = {
        sigma16.InstructionRRR: rrr_handler,
        sigma16.InstructionRX: rx_handler,
        sigma16.InstructionEXP0: exp0_handler,
    }
    dispatch[type(instruction)](instruction)

def main() -> None:
    emu = sigma16.Emulator("array_sum.bin", trace_handler=handler)
    emu.execute()

if __name__ == "__main__":
    main()
```


The executable is the raw machine code produced by the [official](https://jtod.github.io/home/Sigma16/releases/3.1.2/app/Sigma16.html) Sigma16 assembler. A demonstration of the emulator usage is shown below.

![example usage](https://raw.githubusercontent.com/birb007/sigma16-emulator/master/demo/demo.png)

## Customisation

You can disable/cutomise various features by modifying `config.h`. Additionally, a user can modify `tracing.c` to include their own tracing functionality.

# Tooling

## Assembler

The assembler will assemble a specified source file into a binary to be ran under the emulator. The assembler ignores all whitespace and is **case insensitive**. An example application is written below:

```armasm
start:
    lea r1, 0[r0]   ; fibonacci number
    lea r2, 1[r0]
    lea r3, 10[r0]  ; counter
    lea r4, 1[r0]
fib:
    add r5, r1, r2
    lea r1, 0[r2]
    lea r2, 0[r5]

    sub r3, r3, r4
    lea r6, 123[r0]
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

We have successfully calculated the 10th fibonacci number, 55. The assembler uses the same mnemonics as the official emulator. However, labels must be of the form `[a-Z]:` and must be unique (excluding case).

## Disassembler

The assembler will disassemble a specified binary file into a textual representation including the raw binary, file offset, Sigma16 memory offset, and the decoded mnemonics (if any). An example is shown below.

![disassembler output](https://raw.githubusercontent.com/birb007/sigma16-emulator/master/demo/disasm.png)

`[file offset|sigma16 offset] <raw hex> <mnemonic>`

The colours are intentional:

```
Yellow  opcode
Magenta destination
Blue    sa register
Green   sb register
Cyan    data
```

If the disassembler fails to decode instructions it will assume it is data. However, the disassembler will aggressively attempt to decode the entire file and display appropriately.
