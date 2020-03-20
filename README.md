# Alternative Sigma16 Implementation

This is an alternative, CLI based, emulator for Sigma16. Sigma16 is a research architecture developed by [John T. O'Donnell](https://github.com/jtod) and used for the systems course at University of Glasgow. This project also includes Python bindings which allow for high performance but greater accessibility.

While the core ISA has been implemented, the following have **not** been implemented:
- EXP instructions
- The `inv` instruciton

Installation and build instructions for C version (includes tracing):
```
$ git clone https://github.com/birb007/sigma16-emulator.git
$ cd sigma16-emulator
$ make
```

A `sigma16` executable should then be present in the main repository directory. To use the emulator, specify an executable in the command line arguments.
```
usage: ./sigma16 [filename]
```

Installation and build instructions for Python extension:
```
$ git clone https://github.com/birb007/sigma16-emulator.git
$ cd sigma16-emulator/src
$ python setup.py install
```

Example
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

This will create a shared object which the Python interpreter can load, this provides some convenience functions for Python.

The executable is the raw machine code produced by the [official](https://jtod.github.io/home/Sigma16/releases/3.1.2/app/Sigma16.html) Sigma16 assembler. A demonstration of the emulator usage is shown below.

![example usage](https://raw.githubusercontent.com/birb007/sigma16-emulator/master/demo/demo.png)

## Customisation

You can disable/cutomise various features by modifying `Makefile`. The compiler flag `-DENABLE_TRACE` can be removed to disable all interrupts. Additionally, a user can modify `tracing.c` to include their own tracing functionality. Further, you can remove `-DENABLE_TRACE` to remove the tracing functionality from the Python extension.
