# RISC-V Simulator

## Description

RISC-V Simullator as part of CS2323-Computer Architecture Fall 2024 Lab project.

## Table of Contents


- [Implementation](#implementation)
- [Design Decisions](#design-decisions)
- [Capabilities](#capabilities)
- [Cache simulation](#cache-simulation)
- [Issues](#issues)
- [Challenges faced and solutions](#challenges-faced-and-solutions)
- [Improvements](#improvements)


## Implementation

Build on top of a RISC-V Assembler made in previous lab. 
The main runner is a VM struct 
```C
typedef struct VM {
    char filename[100];
    input_s *input;
    registers_s *register_file;
    memory_s *memory;
    lookup_table_s *lookup_table;
    uint64_t *breakpoints;
    uint64_t pc;
    int64_t last_pc;
    stack_s *call_stack;
    cache_s *cache;
} vm_s;
```

The input struct contains the input file, input binary, and a map containing line number, instruction number and label reference (if any).  
On load, the vm assembles the input file, initializes the memory with binary, registers, breakpoints, pc, call stack and cache.


## Design decisions

- For executing binary from memory a lookup table has been implemented to effciently exectute the instructions. Use of hash function keeps the size of lookup table minimal.


## Capabilities

- Assembler which can assemble the following RISC-V instructions:
    - R-type
        - add
        - sub
        - xor
        - or
        - and
        - sll
        - srl
        - sra
        - slt
        - sltu
    - I-type
        - addi
        - xori
        - ori
        - andi
        - slli
        - srli
        - srai
        - slti
        - sltiu
        - lb
        - lh
        - lw
        - ld
        - lbu
        - lhu
        - lwu
        - jalr
    - S-type
        - sb
        - sh
        - sw
        - sd
    - B-type
        - beq
        - bne
        - blt
        - bge
        - bltu
        - bgeu
    - U-type
        - lui
        - auipc
    - J-type
        - jal
    - Pseudo instructions
        - nop
        - mv
        - not
        - neg
        - j
- Assembler being able to handle:
    - .data directive
    - .text directive
    - .dword
    - .word
    - .half
    - .byte
    - Decimal, Hexadecimal, Binary numbers
- Assembler having robust error detection:
    - Shows the file:line_number:character_number for the error
    - Incorrect instruction format: Shows rhe correct format for the said instruction
    - Incorrect register names
    - Out of bound immediate values
    - Label not found
    - Standalone tokens
- Simulator having the following capabilities:
    - Simulation of RISC-V programs along with register and memory simulation
    - Call stack support
    - Breakpoints support
    - Cache simulation

## Cache simulation
Given a config file can simulate the cache. Cache is disabled by default
```C
typedef struct cache_line {
    uint64_t tag;
    uint8_t *block;
    uint64_t valid;
    uint64_t dirty;
    uint64_t last_used;
    uint64_t arrival_time;
} cache_line_s;


typedef struct cache_struct {
    _Bool enabled;                  

    uint64_t cache_size;            
    uint64_t block_size;            
    uint64_t associativity;         
    char replacement_policy[10];    
    char write_policy[10];          

    uint64_t num_sets;              
    uint64_t num_lines;
    uint64_t index_bits;            
    uint64_t byte_offset_bits;      
    uint64_t tag_bits;

    cache_line_s **lines;           
    cache_line_s **sets;           

    uint64_t access;                
    uint64_t hit;                   
    uint64_t miss;                 

} cache_s;
```

Replacement policies supported:
- Least recently used (LRU)
- First in first out (FIFO)
- Random (RAND)

Write policies supported:
- Write Through, no write allocate (WT)
- Write Back, write allocate (WB)

### Usage

`cache enable config_file` enables the cache with the given config file.   
config file should contain the following fields sequentially:
- cache_size
- block_size
- associativity
- replacement_policy
- write_policy

`cache disable` disables the cache.

`cache status` shows the current status of the cache.

`cache invalidate` invalidates the cache.

`cache dump dump_file` dumps the cache state to the given file.

`cache stats` shows the cache statistics.


## Issues

- The file editing support in not robust
- There is no step back functionality
- There might be bugs in the simulator as the testing has not been thorough


## Challenges faced and solutions

No challenges faced

## Improvements

[Demonstration Video](https://www.youtube.com/watch?v=AGLSIv_ldP0)

Implemented a terminal GUI for the simulator using no external libraries. All the functionality has been written from scratch using the ncurses and pthreads libraries.  

The GUI has the following features:

- Code running visualization along with syntax highlighting
- Register and memory visualization
- Breakpoints visualization
- Cache statistics visualization
- Call stack visualization
- Step by step executions via buttons (s key or right arrow key)
- Reset button (r key)
- Run button (w key and space key)
- Memory edit functionality
- All the cache functionality via buttons
- In-built code editing functionality
- Scroll functionality for code and memory visualization
- Exit button (q key or click on the exit button)

WARNING: The exiting must be strictly done via the exit button. Exiting via the terminal may cause terminal buffer corruption

### Implementation

Modeled the simulator as  DFA with the states as nodes and the transitions as edges.

- Used ncurses for the GUI
- Used pthreads for the GUI and the simulator to run concurrently
- Used shared memory for the GUI and the simulator to communicate
- Used mutexes for synchronization
- Two worker threads for the `gui_handler` and `input_handler`
- The code can be found in `gui_interface.c` and `gui_interface.h`