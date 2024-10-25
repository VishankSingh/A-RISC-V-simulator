
# RISC-V Assembler

## Description

RISC-V Simullator as part of CS2323-Computer Architecture Fall 2024 Lab project.

## Usage

Type `make` in the main directory to build.

Type `./riscv_sim` to execute.  

### Commands
- `load <filename>` / `ld <filename>`  
    Load a file into the simulator for execution.

- `run` / `r`   
    Run the loaded program from the beginning.

- `step` / `s`  
    Step through the program one instruction at a time.

- `regs`  
    Print the current values of all the registers.

- `mem <address> <size>`  
    Print the contents of memory starting from the specified address for the given size.

- `break <line number>`  
    Set a breakpoint at the specified line number in the program.

- `del-break <line number>`  
    Delete the breakpoint set at the specified line number.

- `breakpoints`  
    Print a list of all the currently set breakpoints.

- `show-stack` / `stack`  
    Print the current call stack.

- `pc`  
    Print the current value of the program counter.

- `input`  
    Print the contents of the input file loaded into the simulator.

- `reset` / `rst`  
    Reset the simulator, clearing the state and allowing a fresh start.

- `mem_write <address> <data> <bytes>`  
    Write the specified data to memory at the given address for a certain number of bytes.

- `exit` / `quit` / `q`  
    Exit the simulator.


## Code base information
- **main.c**   
    caller for assembler

- **assembler.h assembler.c**  
    assembler file creating the pipeline

- **globals.h globals.c**  
    file storing global macros and variable

- **instructions.h instructions.c**  
    storing the instruction bit data and utility functions

- **io.h io.c**  
    contains input output utility functions 

- **lexer.h lexer.c**  
    lexer which tokenizes the input code
    
- **parser.h parser.c**  
    takes tokenlist and check for errors and uses code generation code to generate hex

- **tokens.h tokens.c**  
    token and token list struct and their utility functions

- **utils.h utils.c**  
    contains utility functions needed by all files

- **memory.h memory.c**  
    stores meta data and simulates little endian memory and makes operations easy via functions 

- **registers.h registers.c**  
    simulates 64 bit registers

- **stack.h stack.c**  
    stack implementation for call stack

- **vm_input.h vm_input.c**  
    helps get the input for the vm from the assembler, stores the meta data and the code in string and hex format

- **vm.h vm.c**  
    runs the whole simulation and takes command
    

