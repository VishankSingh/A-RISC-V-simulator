#include "../include/gui_interface.h"
#include "../include/gui_utils.h"


#include <stdlib.h>
#include <string.h>
#include <signal.h>

_Bool is_file_loaded = 0;
_Bool is_main = 1;
_Bool is_exit = 0;
_Bool is_input = 0;
_Bool is_edit = 0;
_Bool is_error_show = 0;
_Bool is_mem_show = 0;
_Bool is_mem_edit = 0;
_Bool is_top_menu_load = 0;
_Bool is_top_menu_mem_show = 0;
_Bool is_top_menu_mem_edit = 0;
_Bool is_cache_details_show = 0;

char input_buffer[BUFFER_SIZE];
int input_index = 0;
char input[BUFFER_SIZE];
uint64_t mem_start_addr = 0;
uint64_t code_print_start = 0;
uint64_t mem_print_start = 0x00000;
char error_msg[100];

WINDOW *top_menu;
WINDOW *load_dialog;
WINDOW *mem_show_dialog;
WINDOW *mem_edit_dialog;
WINDOW *error_dialog;
WINDOW *code_window;
WINDOW *register_window;
WINDOW *memory_window;
WINDOW *breakpoints_window;
WINDOW *stack_window;
WINDOW *cache_window;
WINDOW *cache_details_window;

WINDOW *art_window;

uint64_t cursor_x = 0;
uint64_t cursor_y = 0;

FILE *code_file_ = NULL;
uint64_t file_size = 0;

MEVENT event;

const char *keywords[] = {
    "add", "sub", "xor", "or", "and", "sll", "srl", "sra", "slt", "sltu",
    "addi", "xori", "ori", "andi", "slli", "srli", "srai", "slti", "sltiu",
    "lb", "lh", "lw", "ld", "lbu", "lhu", "lwu",
    "sb", "sh", "sw", "sd",
    "beq", "bne", "blt", "bge", "bltu", "bgeu",
    "jal", "jalr",
    "lui", "auipc",
    "nop", "mv", "not", "neg", "j"
};
const int keyword_colors[] = {
    COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9),
    COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), 
    COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), 
    COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), 
    COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), 
    COLOR_PAIR(9), COLOR_PAIR(9), 
    COLOR_PAIR(9), COLOR_PAIR(9), 
    COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9), COLOR_PAIR(9)
};

const char *register_aliases[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};


void gui_init() {
    initscr();
    start_color();
    use_default_colors();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS | BUTTON1_CLICKED | BUTTON2_CLICKED | BUTTON3_CLICKED | BUTTON4_CLICKED | BUTTON5_CLICKED | REPORT_MOUSE_POSITION, NULL);
    mouseinterval(0);
    
    curs_set(0);

    signal(SIGWINCH, handle_resize);

    #define COLOR_LIGHT_ORANGE 110
    init_color(COLOR_LIGHT_ORANGE, 1000, 686, 303);

    #define COLOR_NEW_CYAN 111
    init_color(COLOR_NEW_CYAN, 220, 1000, 961);

    #define COLOR_NEW_PINK 112
    init_color(COLOR_NEW_PINK, 882, 529, 1000);

    #define COLOR_LIGHT_GRAY 113
    init_color(COLOR_LIGHT_GRAY, 550, 550, 550);
    
    #define COLOR_DARK_GRAY 114
    init_color(COLOR_DARK_GRAY, 300, 300, 300);

    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_YELLOW, -1);
    init_pair(4, COLOR_BLUE, -1);
    init_pair(5, COLOR_MAGENTA, -1);
    init_pair(6, COLOR_CYAN, -1);
    init_pair(7, COLOR_WHITE, -1);
    init_pair(8, COLOR_LIGHT_ORANGE, -1);
    init_pair(9, COLOR_NEW_CYAN, -1);
    init_pair(10, COLOR_NEW_PINK, -1);
    init_pair(11, COLOR_LIGHT_GRAY, -1);
    init_pair(12, COLOR_DARK_GRAY, -1);
    
    


    init_pair(50, COLOR_WHITE, COLOR_GREEN);
    init_pair(51, COLOR_WHITE, COLOR_YELLOW);
    init_pair(52, COLOR_BLACK, COLOR_WHITE);


    top_menu = init_window(3, COLS, 0, 0);
    code_window = init_window(LINES - 3, 2 * COLS / 3, 3, 0); // 2 * COLS / 3 = 140
    register_window = init_window(34, 37, 3, 174);
    load_dialog = init_window(7, 61, 20, 75);
    mem_show_dialog = init_window(7, 61, 20, 75);
    mem_edit_dialog = init_window(7, 61, 20, 75);
    error_dialog = init_window(7, 61, 20, 75);
    memory_window = init_window(45, 110, 7, 53);
    breakpoints_window = init_window(20, 34, 3, 140);
    stack_window = init_window(32, 34, 23, 140);
    cache_window = init_window(6, 37, 37, 174);
    cache_details_window = init_window(11, 33, 20, 91);
    // mem_edit_dialog = init_window(LINES - 6, COLS / 2, 6, COLS / 2);

    art_window = init_window(12, 37, 43, 174);

    wattron(top_menu, A_BOLD);


    
    memset(input_buffer, 0, BUFFER_SIZE);
    memset(input, 0, BUFFER_SIZE);



}

void handle_resize() {
    endwin();
    refresh();
    clear();
}

void end_gui() {
    if (top_menu != NULL) {
        delwin(top_menu);
        top_menu = NULL;
    }
    if (load_dialog != NULL) {
        delwin(load_dialog);
        load_dialog = NULL;
    }
    if (mem_show_dialog != NULL) {
        delwin(mem_show_dialog);
        mem_show_dialog = NULL;
    }
    if (mem_edit_dialog != NULL) {
        delwin(mem_edit_dialog);
        mem_edit_dialog = NULL;
    }
    if (code_window != NULL) {
        delwin(code_window);
        code_window = NULL;
    }
    if (register_window != NULL) {
        delwin(register_window);
        register_window = NULL;
    }
    if (memory_window != NULL) {
        delwin(memory_window);
        memory_window = NULL;
    }
    curs_set(1);
    echo();
    
    endwin();
}

void draw_top_menu() {
    wclear(top_menu);
    wattron(top_menu, COLOR_PAIR(7));
    mvwprintw(top_menu, 1, 2, "LOAD    MEM SHOW    MEM EDIT    EXIT");
    mvwprintw(top_menu, 1, 146, "TOGGLE CACHE    CACHE DETAILS    INVALIDATE CACHE    CACHE DUMP");
    box(top_menu, 0, 0);
    wattroff(top_menu, COLOR_PAIR(7));
    wrefresh(top_menu);
}

void draw_code_window(char* filename, vm_s* vm) {
    wclear(code_window);
    wattron(code_window, COLOR_PAIR(7));

    if (filename == NULL) {
        box(code_window, 0, 0);
        mvwprintw(code_window, 0, 2, "Code window");
        wattroff(code_window, COLOR_PAIR(7));
        wrefresh(code_window);
        return;
    }
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        box(code_window, 0, 0);
        mvwprintw(code_window, 0, 2, "Code window");
        wattroff(code_window, COLOR_PAIR(7));
        wrefresh(code_window);
        return;
    }
    
    wattron(code_window, A_BOLD);
    mvwprintw(code_window, 1, 2, "%s", vm->filename);
    mvwprintw(code_window, 2, 2, "Fs: %ld", file_size);
    wattroff(code_window, A_BOLD);

    if (is_edit) {
        wattron(code_window, A_BOLD);
        mvwprintw(code_window, 1, 122, "SAVE");
        mvwprintw(code_window, 1, 132, "CANCEL");
        mvwprintw(code_window, 2, 122, "LINE:%ld, COL:%ld", cursor_x + 1, cursor_y + 1);
        
        wattroff(code_window, A_BOLD);
        char line[130];
        uint64_t i = 0;
        while (fgets(line, sizeof(line), file)) {
            if (i >= code_print_start && i < code_print_start + LINES - 6) {
                if (i == cursor_y) {
                    wattron(code_window, A_BOLD);
                    mvwprintw(code_window, 3 + i - code_print_start, 5, "%3ld | %s", i + 1, line);
                    wattroff(code_window, A_BOLD);
                } else {
                    mvwprintw(code_window, 3 + i - code_print_start, 5, "%3ld | %s", i + 1, line);
                }
            }
            i++;
        }
        
    } else {
        wattron(code_window, A_BOLD);
        mvwprintw(code_window, 1, 122, "TOGGLE EDIT MODE");
        wattroff(code_window, A_BOLD);

        char line[130];
        uint64_t i = 0;
        while (fgets(line, sizeof(line), file)) {
            uint64_t curr_line = get_linenum_instnum(vm->input->lil_map, vm->pc / 4 + 1);
            if (i >= code_print_start && i < code_print_start + LINES - 7) {
                if (i == curr_line - 1) {
                    wattron(code_window, A_BOLD);
                    wattron(code_window, COLOR_PAIR(50));
                    mvwprintw(code_window, 3 + i - code_print_start, 5, "%3ld | %s", i + 1, line);
                    wattroff(code_window, A_BOLD);
                    wattroff(code_window, COLOR_PAIR(50));
                } else {
                    wattron(code_window, A_BOLD);
                    mvwprintw(code_window, 3 + i - code_print_start, 5, "%3ld | ", i + 1);
                    wattroff(code_window, A_BOLD);
                    int j = 0;
                    while (line[j] == ' ') {
                        j++;
                    }
                    _Bool comment = 0;
                    char *token = strtok(line, " ");
                    while (token != NULL) {
                        if (token[0] == '#')
                            comment = 1;

                        while(line[j] == ' ')
                            j++;


                        if (!comment) {
                            if (token[strlen(token) - 2] == ':') {
                                wattron(code_window, COLOR_PAIR(10));
                                wattron(code_window, A_BOLD);
                                mvwprintw(code_window, 3 + i - code_print_start, 11 + j, "%s ", token);
                                wattroff(code_window, A_BOLD);
                                wattroff(code_window, COLOR_PAIR(10));
                            } else if (token[0] == '.') {
                                wattron(code_window, COLOR_PAIR(8));
                                wattron(code_window, A_BOLD);
                                mvwprintw(code_window, 3 + i - code_print_start, 11 + j, "%s ", token);
                                wattroff(code_window, A_BOLD);
                                wattroff(code_window, COLOR_PAIR(8));
                            }
                            
                            
                            
                             else {
                                int k = 0;
                                _Bool printed = 0;
                                while (k < sizeof(keywords) / sizeof(keywords[0])) {
                                    if (strcmp(token, keywords[k]) == 0) {
                                        wattron(code_window, keyword_colors[k]);
                                        wattron(code_window, A_BOLD);
                                        mvwprintw(code_window, 3 + i - code_print_start, 11 + j, "%s ", token);
                                        wattroff(code_window, A_BOLD);
                                        wattroff(code_window, keyword_colors[k]);
                                        printed = 1;
                                        break;
                                    }
                                    k++;
                                }


                                if (!printed) {
                                    wattron(code_window, A_BOLD);
                                    mvwprintw(code_window, 3 + i - code_print_start, 11 + j, "%s ", token);
                                    wattroff(code_window, A_BOLD);
                                }
                            }
                        }
                        
                        else {
                            wattron(code_window, COLOR_PAIR(11));
                            mvwprintw(code_window, 3 + i - code_print_start, 11 + j, "%s", token);
                            wattroff(code_window, COLOR_PAIR(11));
                        }
                        




                        
                        j += strlen(token) + 1;
                        token = strtok(NULL, " ");
                    }
                    



                    //mvwprintw(code_window, 3 + i - code_print_start, 5, "%3ld | %s", i + 1, line);
                }
                

                uint64_t inst_num = get_instnum_linenum(vm->input->lil_map, i + 1);

                if (in_breakpoint(vm, 0x00000 + 4 * (inst_num - 1))) {
                    wattron(code_window, COLOR_PAIR(1));
                    wattron(code_window, A_BOLD);
                    mvwprintw(code_window, 3 + i - code_print_start, 2, "*");
                    wattroff(code_window, A_BOLD);
                    wattroff(code_window, COLOR_PAIR(1));
                }

            }
            i++;
        }
    }


    
    fclose(file);

    

    box(code_window, 0, 0);
    mvwprintw(code_window, 0, 2, "Code window");
    move(cursor_y - code_print_start + 6, cursor_x + 11);
    wattroff(code_window, COLOR_PAIR(7));
    wrefresh(code_window);
}

void draw_load_dialog(char* input, int input_index) {
    wclear(load_dialog);
    wattron(load_dialog, COLOR_PAIR(7));
    mvwprintw(load_dialog, 1, 58, "X");
    mvwprintw(load_dialog, 3, 2, "%s", input);
    move(23, 75 + input_index + 2);
    wattron(load_dialog, COLOR_PAIR(52));
    mvwprintw(load_dialog, 1, 25, " Load file ");
    wattroff(load_dialog, COLOR_PAIR(52));
    wattron(load_dialog, COLOR_PAIR(7));
    box(load_dialog, 0, 0);
    wattroff(load_dialog, COLOR_PAIR(7));
    wrefresh(load_dialog);
}

void draw_mem_show_dialog(char* input, int input_index) {
    wclear(mem_show_dialog);
    wattron(mem_show_dialog, COLOR_PAIR(7));
    mvwprintw(mem_show_dialog, 1, 58, "X");
    mvwprintw(mem_show_dialog, 3, 2, "%s", input);
    move(23, 75 + input_index + 2);
    wattron(mem_show_dialog, COLOR_PAIR(52));
    mvwprintw(mem_show_dialog, 1, 7, " Type starting address <= 0x50000 (Eg: 0x10000) ");
    wattroff(mem_show_dialog, COLOR_PAIR(52));
    wattron(mem_show_dialog, COLOR_PAIR(7));
    box(mem_show_dialog, 0, 0);
    wattroff(mem_show_dialog, COLOR_PAIR(7));
    wrefresh(mem_show_dialog);
}

void draw_mem_edit_dialog(char* input, int input_index) {
    wclear(mem_edit_dialog);
    wattron(mem_edit_dialog, COLOR_PAIR(7));
    mvwprintw(mem_edit_dialog, 1, 58, "X");
    mvwprintw(mem_edit_dialog, 3, 2, "%s", input);
    move(23, 75 + input_index + 2);
    wattron(mem_edit_dialog, COLOR_PAIR(52));
    mvwprintw(mem_edit_dialog, 1, 7, " Edit memory (Format: <address> <data> <bytes>) ");
    wattroff(mem_edit_dialog, COLOR_PAIR(52));
    wattron(mem_edit_dialog, COLOR_PAIR(7));
    box(mem_edit_dialog, 0, 0);
    wattroff(mem_edit_dialog, COLOR_PAIR(7));
    wrefresh(mem_edit_dialog);
}

void draw_register_window(vm_s* vm) {
    wclear(register_window);
    wattron(register_window, COLOR_PAIR(7));
    for(uint64_t i = 0; i < 32; i++) {
        if (i == vm->register_file->last_changed) {
            wattron(register_window, A_BOLD);
            wattron(register_window, COLOR_PAIR(50));
            mvwprintw(register_window, i + 1, 2, " x%-3ld| %-4s | 0x%016lx ", i, register_aliases[i], vm->register_file->reg_arr[i]);
            wattroff(register_window, A_BOLD);
            wattroff(register_window, COLOR_PAIR(50));
        } else {
            mvwprintw(register_window, i + 1, 2, " x%-3ld| %-4s | 0x%016lx ", i, register_aliases[i], vm->register_file->reg_arr[i]);
        }
    }
    box(register_window, 0, 0);
    mvwprintw(register_window, 0, 2, "Registers");
    wattroff(register_window, COLOR_PAIR(7));
    wrefresh(register_window);
}

void draw_memory_window(vm_s* vm) {
    wclear(memory_window);
    wattron(memory_window, COLOR_PAIR(7));
    mvwprintw(memory_window, 1, 107, "X");
    mvwprintw(memory_window, 1, 2, "Memory");

    wattron(memory_window, A_BOLD);
    mvwprintw(memory_window, 3, 4, "Address |         Word         | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 ");
    mvwprintw(memory_window, 4, 4, "--------|----------------------|--------|--------|--------|--------|--------|--------|--------|--------");
    int j = 0;
    for (uint64_t i = mem_print_start; i <= vm->memory->size && j < 37; i+=8) {
        wattron(memory_window, COLOR_PAIR(2));
        mvwprintw(memory_window, j + 5, 4, 
            "0x%05lx ", 
            i);
        wattroff(memory_window, COLOR_PAIR(2));

        mvwprintw(memory_window, j + 5, 12, "|  ");

        if (!vm->memory->change[i] && 
        !vm->memory->change[i + 1] && 
        !vm->memory->change[i + 2] && 
        !vm->memory->change[i + 3] && 
        !vm->memory->change[i + 4] && 
        !vm->memory->change[i + 5] && 
        !vm->memory->change[i + 6] && 
        !vm->memory->change[i + 7]) {
            wattron(memory_window, COLOR_PAIR(12));
        }

        mvwprintw(memory_window, j + 5, 12 + 3, 
            "0x%016lx  ", 
            read_memory(vm->memory, i)
                     | read_memory(vm->memory, i + 1) << 8
                     | read_memory(vm->memory, i + 2) << 16
                     | read_memory(vm->memory, i + 3) << 24
                     | read_memory(vm->memory, i + 4) << 32
                     | read_memory(vm->memory, i + 5) << 40
                     | read_memory(vm->memory, i + 6) << 48
                     | read_memory(vm->memory, i + 7) << 56);

        if (!vm->memory->change[i] && 
        !vm->memory->change[i + 1] && 
        !vm->memory->change[i + 2] && 
        !vm->memory->change[i + 3] && 
        !vm->memory->change[i + 4] && 
        !vm->memory->change[i + 5] && 
        !vm->memory->change[i + 6] && 
        !vm->memory->change[i + 7]) {
            wattroff(memory_window, COLOR_PAIR(12));
        }

        for (int l = 0; l < 8; l++) {
            mvwprintw(memory_window, j + 5, 12 + 3 + 2 + 16 + 2 + 9 * l, 
                "|  ");

            if (!vm->memory->change[i + l]) {
                wattron(memory_window, COLOR_PAIR(12));
            }
            mvwprintw(memory_window, j + 5, 12 + 3 + 2 + 16 + 2 + 9 * l + 3, 
                "0x%02x  ", 
                vm->memory->data[i + l]);
            if (!vm->memory->change[i + l]) {
                wattroff(memory_window, COLOR_PAIR(12));
            }
        }


        //mvwprintw(memory_window, j + 5, 12, 
        //    "|  0x%016lx  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  ", 
        //    read_memory(vm->memory, i)
        //             | read_memory(vm->memory, i + 1) << 8
        //             | read_memory(vm->memory, i + 2) << 16
        //             | read_memory(vm->memory, i + 3) << 24
        //             | read_memory(vm->memory, i + 4) << 32
        //             | read_memory(vm->memory, i + 5) << 40
        //             | read_memory(vm->memory, i + 6) << 48
        //             | read_memory(vm->memory, i + 7) << 56,
        //    vm->memory->data[i],
        //    vm->memory->data[i + 1],
        //    vm->memory->data[i + 2],
        //    vm->memory->data[i + 3],
        //    vm->memory->data[i + 4],
        //    vm->memory->data[i + 5],
        //    vm->memory->data[i + 6],
        //    vm->memory->data[i + 7]
        //);
        j++;
    }
        wattroff(memory_window, A_BOLD);
    box(memory_window, 0, 0);
    wattroff(memory_window, COLOR_PAIR(7));
    wrefresh(memory_window);
}

void draw_breakpoints_window(vm_s *vm) {
    wclear(breakpoints_window);
    wattron(breakpoints_window, COLOR_PAIR(7));
    wattron(breakpoints_window, COLOR_PAIR(1));
    mvwprintw(breakpoints_window, 18, 2, "Remove all");
    wattroff(breakpoints_window, COLOR_PAIR(1));
    wattron(breakpoints_window, A_BOLD);
    int j = 0;
    for (int i = 1; i < MAX_BREAKPOINTS; i++) {
        if (vm->breakpoints[i] != INT64_MAX) {
            wattron(breakpoints_window, COLOR_PAIR(2));
            int line_no = get_linenum_instnum(vm->input->lil_map, vm->breakpoints[i] / 4 + 1);
            mvwprintw(breakpoints_window, j + 2, 2, "Line %d, 0x%05lx", line_no, vm->breakpoints[i]);
            wattroff(breakpoints_window, COLOR_PAIR(2));
            j++;
        }
    }
    wattroff(breakpoints_window, A_BOLD);
    box(breakpoints_window, 0, 0);
    mvwprintw(breakpoints_window, 0, 2, "Breakpoints");
    wattroff(breakpoints_window, COLOR_PAIR(7));
    wrefresh(breakpoints_window);
}

void draw_stack_window(vm_s *vm) {
    wclear(stack_window);
    wattron(stack_window, COLOR_PAIR(7));
    wattron(stack_window, A_BOLD);
    int j = 0;

    int64_t curr_line;
    if (vm->last_pc == 0x00000 - 4) {
        curr_line = 0;
    } else {
        curr_line = get_linenum_instnum(vm->input->lil_map, vm->last_pc / 4 + 1);
    }
    if (curr_line != -1 && !is_empty(vm->call_stack)) {
        
    }
    for (int i = 0; i <= vm->call_stack->top; i++) {
        mvwprintw(stack_window, j + 1, 2, "%s:%d", vm->call_stack->data[i].label, vm->call_stack->data[i].line_num);
        j++;
    }


    wattroff(stack_window, A_BOLD);
    box(stack_window, 0, 0);
    mvwprintw(stack_window, 0, 2, "Stack");
    wattroff(stack_window, COLOR_PAIR(7));
    wrefresh(stack_window);
}

void draw_cache_window(vm_s *vm) {
    wclear(cache_window);
    wattron(cache_window, COLOR_PAIR(7));
    box(cache_window, 0, 0);
    mvwprintw(cache_window, 0, 2, "Cache");

    mvwprintw(cache_window, 1, 2, "Status   | ");
    if (vm->cache->enabled) {
        wattron(cache_window, COLOR_PAIR(2));
        mvwprintw(cache_window, 1, 13, "Enabled");
        wattroff(cache_window, COLOR_PAIR(2));
    } else {
        wattron(cache_window, COLOR_PAIR(1));
        mvwprintw(cache_window, 1, 13, "Disabled");
        wattroff(cache_window, COLOR_PAIR(1));
    }
    mvwprintw(cache_window, 2, 2, "Hits     | %ld", vm->cache->hit);
    mvwprintw(cache_window, 3, 2, "Misses   | %ld", vm->cache->miss);
    if (vm->cache->miss == 0) {
        mvwprintw(cache_window, 4, 2, "Hit rate | 0");
    } else {
        mvwprintw(cache_window, 4, 2, "Hit rate | %lf", (float)vm->cache->hit / (float)(vm->cache->access));
    }
    wattroff(cache_window, COLOR_PAIR(7));
    wrefresh(cache_window);
}

void draw_cache_details_window(vm_s *vm) {
    wclear(cache_details_window);
    wattron(cache_details_window, COLOR_PAIR(7));
    wattron(cache_details_window, COLOR_PAIR(52));
    mvwprintw(cache_details_window, 1, 9, " Cache details ");
    wattroff(cache_details_window, COLOR_PAIR(52));

    mvwprintw(cache_details_window, 1, 30,  "X");

    mvwprintw(cache_details_window, 3, 2,  "Cache status       | %s", (vm->cache->enabled ? "Enabled" : "Disabled"));

    mvwprintw(cache_details_window, 4, 2,  "Cache size         | %ld", vm->cache->cache_size);
    mvwprintw(cache_details_window, 5, 2,  "Block size         | %ld", vm->cache->block_size);
    mvwprintw(cache_details_window, 6, 2,  "Associativity      | %ld", vm->cache->associativity);
    mvwprintw(cache_details_window, 7, 2,  "Replacement policy | %s", vm->cache->replacement_policy);
    mvwprintw(cache_details_window, 8, 2,  "Write policy       | %s", vm->cache->write_policy);


    box(cache_details_window, 0, 0);
    wattroff(cache_details_window, COLOR_PAIR(7));
    wrefresh(cache_details_window);
}

void draw_art_window() {
    box(art_window, 0, 0);
    mvwprintw(art_window, 1, 2, "   \\ ");
    mvwprintw(art_window, 2, 2, "    '-.__.-'");
    mvwprintw(art_window, 3, 2, "    /oo |--.--,--,--.");
    mvwprintw(art_window, 4, 2, "    \\_.-'._i__i__i_.'");
    mvwprintw(art_window, 5, 2, "          \"\"\"\"\"\"\"\"\"\"  ");
    wrefresh(art_window);

}


void insert_newline() {
    FILE *code_file = fopen(temp_file, "r+");
    char line[110];  
    char new_line[110];
    FILE *tfile = fopen("tmpppppp", "w+");
    for (int64_t i = 0; i < (int64_t)cursor_y; i++) {
        if (fgets(line, sizeof(line), code_file) != NULL) {
            fputs(line, tfile);
        }
    }

    if (fgets(line, sizeof(line), code_file) != NULL) {
        strcpy(new_line, &line[cursor_x]);  
        line[cursor_x] = '\0';  
        fputs(line, tfile);
        fputc('\n', tfile);  
        fputs(new_line, tfile);  
    }

    while (fgets(line, sizeof(line), code_file)) {
        fputs(line, tfile);
    }

    fclose(tfile);
    remove(temp_file);
    rename("tmpppppp", temp_file);
    fclose(code_file);
    file_size++;

    cursor_x = 0;
    if (cursor_y < file_size) {
        cursor_y++;
        if (cursor_y > code_print_start + LINES - 8) {
            code_print_start = cursor_y - LINES + 8;
        }
    }

}

void handle_backspace(FILE *code_file_) {

    if (cursor_x == 0 && cursor_y == 0) {
        return;
    }

    fseek(code_file_, 0, SEEK_SET);

    char line[101];

    FILE *tfile = fopen("tmpppppp", "w+");

    for (int64_t i = 0; i < (int64_t)cursor_y - 1; i++) {
        if (fgets(line, sizeof(line), code_file_) != NULL) {
            fputs(line, tfile);
        }
    }

    // TODO: Handle backspace at the start of the line (join with previous line)
    // Handle backspace at the start of the line (join with previous line)
    if (cursor_x == 0 && cursor_y > 0) {
        char prev_line[101] = {0};
        char curr_line[101] = {0};

        // Read previous and current lines
        fgets(prev_line, sizeof(prev_line), code_file_);
        fgets(curr_line, sizeof(curr_line), code_file_);

        // Remove newline from previous line if it exists, then concatenate
        size_t prev_len = strlen(prev_line);
        if (prev_len > 0 && prev_line[prev_len - 1] == '\n') {
            prev_line[prev_len - 1] = '\0';
        }
        strcat(prev_line, curr_line);  // Concatenate current line to previous line

        // Write the merged line to the temp file
        fputs(prev_line, tfile);
        cursor_y--;
        if (code_print_start > 0 && cursor_y < code_print_start) {
            code_print_start = cursor_y;
        }
        cursor_x = prev_len - 1;
        file_size--;
    } else { 
        if (cursor_y > 0) {

        fgets(line, sizeof(line), code_file_);
        fputs(line, tfile);
        }
        // Standard backspace within a line (not at line start)
        fgets(line, sizeof(line), code_file_);  // Read current line
        size_t len = strlen(line);
        if (cursor_x > 0 && len > 0) {
            memmove(&line[cursor_x - 1], &line[cursor_x], len - cursor_x);
            line[len - 1] = '\0';  // Null-terminate after deletion
        }
        fputs(line, tfile);  // Write modified line to temp file
        cursor_x--;
    }


    while (fgets(line, sizeof(line), code_file_)) {
        fputs(line, tfile);
    }

    fclose(tfile);
    remove(temp_file);
    rename("tmpppppp", temp_file);

}

void handle_space(FILE *code_file_) {
    char line[110]; 
    FILE *tfile = fopen("tmpppppp", "w+");
    for (int64_t i = 0; i < (int64_t)cursor_y; i++) {
        if (fgets(line, sizeof(line), code_file_) != NULL) {
            fputs(line, tfile);
        }
    }
    fgets(line, sizeof(line), code_file_);
    
    size_t line_len = strlen(line);
    memmove(&line[cursor_x + 1], &line[cursor_x], line_len - cursor_x);
    line[cursor_x] = ' ';
    line[line_len + 1] = '\0';
    fputs(line, tfile);

    while (fgets(line, sizeof(line), code_file_)) {
        fputs(line, tfile);
    }

    fclose(tfile);
    remove(temp_file);
    rename("tmpppppp", temp_file);

    cursor_x++;
}

void handle_left() {
    if (cursor_x > 0) {
        cursor_x--;
    }
}

void handle_right() {
    FILE *code_file = fopen(temp_file, "r");
    char line[101]; 
    
    for (int64_t i = 0; i < (int64_t)cursor_y; i++) {
        fgets(line, sizeof(line), code_file);
    }
    
    
    if (fgets(line, sizeof(line), code_file) != NULL) {
        size_t line_length = strlen(line);
        
        if (line_length > 0 && line[line_length - 1] == '\n') {
            line_length--;
        }

        if (cursor_x < line_length) {
            cursor_x++;
        }
    }
    fclose(code_file);
}

void handle_up() {
    if (cursor_y > 0) {
        cursor_y--;
        if (code_print_start > 0 && cursor_y < code_print_start) {
            code_print_start = cursor_y;
        }
    }
}

void handle_down() {
    if (cursor_y < file_size - 1) {
        cursor_y++;
        if (cursor_y > code_print_start + LINES - 7) {
            code_print_start = cursor_y - LINES + 7;
        }
    }
}

void insert_char(FILE *code_file_, char ch) {
    char line[110]; 
    FILE *tfile = fopen("tmpppppp", "w+");
    for (int64_t i = 0; i < (int64_t)cursor_y; i++) {
        if (fgets(line, sizeof(line), code_file_) != NULL) {
            fputs(line, tfile);
        }
    }
    fgets(line, sizeof(line), code_file_);
    
    size_t line_len = strlen(line);
    memmove(&line[cursor_x + 1], &line[cursor_x], line_len - cursor_x);
    line[cursor_x] = ch;
    line[line_len + 1] = '\0';
    fputs(line, tfile);

    while (fgets(line, sizeof(line), code_file_)) {
        fputs(line, tfile);
    }

    fclose(tfile);
    remove(temp_file);
    rename("tmpppppp", temp_file);

    cursor_x++;
}












