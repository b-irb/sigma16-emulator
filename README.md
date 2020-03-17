# Alternative Sigma16 Implementation

This is an alternative, CLI based, emulator for Sigma16. Sigma16 is a research architecture developed by [John T. O'Donnell](https://github.com/jtod) and used for the systems course at University of Glasgow.

The core ISA is implemented, including:
- Arithmetic (add, sub, mul, etc.)
- Memory operations (load, store, lea, etc.)
- Branching (jump, jal, etc.)

The following have **not** been implemented:
- EXP instructions
- Interrupts
- The `inv` instruciton

There is no plan to implement the remaining features of Sigma16, this project was purely recreational.

Installation and build instructions:
```
$ git clone https://github.com/birb007/sigma16-emulator.git
$ cd sigma16-emulator
$ make
```

A `sigma16` executable should then be present in the main repository directory. To use the emulator, specify an executable in the command line arguments.
```
usage: ./sigma16 [filename]
```

The executable is the raw machine code produced by the [official](https://jtod.github.io/home/Sigma16/releases/3.1.2/app/Sigma16.html) Sigma16 assembler. A demonstration of the emulator usage is shown below.

![example usage](https://raw.githubusercontent.com/birb007/sigma16-emulator/master/demo/demo.png)

## Customisation

You can disable/cutomise various features by modifying `Makefile`. The compiler flag `-DENABLE_TRACE` can be removed to disable all interrupts. Additionally, a user can modify `tracing.c` to include their own tracing functionality.
