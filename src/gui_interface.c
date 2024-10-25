#include "../include/gui_interface.h"
#include "../include/gui_utils.h"


#include <stdlib.h>
#include <string.h>
#include <signal.h>

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


MEVENT event;

const char *keywords[] = {
    "add", "sub", "mul", "div", "lw", "sw", "beq", "bne", "blt", "bge", "jal", "jalr",
    // ... (add other relevant keywords)
};
const int keyword_colors[] = {
    COLOR_PAIR(2), COLOR_PAIR(2), COLOR_PAIR(2), COLOR_PAIR(2), COLOR_PAIR(3), COLOR_PAIR(3),
    COLOR_PAIR(5), COLOR_PAIR(5), COLOR_PAIR(5), COLOR_PAIR(5), COLOR_PAIR(6), COLOR_PAIR(6),
    // ... (add corresponding color pairs)
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

    init_pair(1, COLOR_WHITE, -1);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_GREEN, -1);
    init_pair(4, COLOR_WHITE, COLOR_GREEN);
    init_pair(5, COLOR_WHITE, COLOR_YELLOW);
    top_menu = init_window(3, COLS, 0, 0);
    code_window = init_window(LINES - 3, 2 * COLS / 3, 3, 0);
    register_window = init_window(34, 37, 3, 174);
    load_dialog = init_window(7, 61, 20, 75);
    mem_show_dialog = init_window(7, 61, 20, 75);
    error_dialog = init_window(7, 61, 20, 75);
    memory_window = init_window(45, 110, 7, 53);
    // mem_edit_dialog = init_window(LINES - 6, COLS / 2, 6, COLS / 2);
    
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
    wattron(top_menu, COLOR_PAIR(1));
    mvwprintw(top_menu, 1, 2, "LOAD    MEM SHOW    MEM EDIT    EXIT");
    box(top_menu, 0, 0);
    wattroff(top_menu, COLOR_PAIR(1));
    wrefresh(top_menu);
}

void draw_code_window(char* filename, vm_s* vm) {
    wclear(code_window);
    wattron(code_window, COLOR_PAIR(1));
    mvwprintw(code_window, 1, 2, "%s", filename);
    if (filename == NULL) {
        box(code_window, 0, 0);
        wattroff(code_window, COLOR_PAIR(1));
        wrefresh(code_window);
        return;
    }
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        box(code_window, 0, 0);
        wattroff(code_window, COLOR_PAIR(1));
        wrefresh(code_window);
        return;
    }
    char line[1024];
    uint64_t i = 0;
    while (fgets(line, sizeof(line), file)) {
        uint64_t curr_line = get_linenum_instnum(vm->input->lil_map, vm->pc / 4 + 1);
        if (i >= code_print_start && i < code_print_start + LINES - 6) {
            if (i == curr_line - 1) {
                wattron(code_window, A_BOLD);
                wattron(code_window, COLOR_PAIR(4));
                mvwprintw(code_window, 3 + i - code_print_start, 5, "%3ld | %s", i + 1, line);
                wattroff(code_window, A_BOLD);
                wattroff(code_window, COLOR_PAIR(4));
            } else {
                mvwprintw(code_window, 3 + i - code_print_start, 5, "%3ld | %s", i + 1, line);
                //mvwprintw(code_window, 3 + i - code_print_start, 5, "%3d | ", i + 1);
                //char *token = strtok(line, " ");
                //int last_token_len = 0;
                //while (token != NULL) {
                //    // Check if the token is a keyword
                //    //int j;
                //    //for (j = 0; j < sizeof(keywords) / sizeof(keywords[0]); j++) {
                //    //    if (strcmp(token, keywords[j]) == 0) {
                //    //        wattron(code_window, keyword_colors[j]);
                //    //        break;
                //    //    }
                //    //}
                //    // Print the token with the appropriate color
                //    mvwprintw(code_window, 3 + i - code_print_start, 5 + strlen("%3d | ") + last_token_len, "%s", token);
                //    //wattroff(code_window, keyword_colors[j]);
                //    last_token_len = strlen(token) + 1;
                //    token = strtok(NULL, " ");
                //}
            }
        }
        i++;
    }
    fclose(file);

    

    box(code_window, 0, 0);
    wattroff(code_window, COLOR_PAIR(1));
    wrefresh(code_window);
}

void draw_load_dialog(char* input, int input_index) {
    wclear(load_dialog);
    wattron(load_dialog, COLOR_PAIR(1));
    mvwprintw(load_dialog, 1, 58, "X");
    mvwprintw(load_dialog, 3, 2, "%s", input);
    move(23, 75 + input_index + 2);
    wattron(load_dialog, COLOR_PAIR(2));
    mvwprintw(load_dialog, 1, 25, " Load file ");
    wattroff(load_dialog, COLOR_PAIR(2));
    wattron(load_dialog, COLOR_PAIR(1));
    box(load_dialog, 0, 0);
    wattroff(load_dialog, COLOR_PAIR(1));
    wrefresh(load_dialog);
}

void draw_mem_show_dialog(char* input, int input_index) {
    wclear(mem_show_dialog);
    wattron(mem_show_dialog, COLOR_PAIR(1));
    mvwprintw(mem_show_dialog, 1, 58, "X");
    mvwprintw(mem_show_dialog, 3, 2, "%s", input);
    move(23, 75 + input_index + 2);
    wattron(mem_show_dialog, COLOR_PAIR(2));
    mvwprintw(mem_show_dialog, 1, 7, " Type starting address <= 0x50000 (Eg: 0x10000) ");
    wattroff(mem_show_dialog, COLOR_PAIR(2));
    wattron(mem_show_dialog, COLOR_PAIR(1));
    box(mem_show_dialog, 0, 0);
    wattroff(mem_show_dialog, COLOR_PAIR(1));
    wrefresh(mem_show_dialog);
}

void draw_register_window(vm_s* vm) {
    wclear(register_window);
    wattron(register_window, COLOR_PAIR(1));
    for(uint64_t i = 0; i < 32; i++) {
        if (i == vm->register_file->last_changed) {
            wattron(register_window, A_BOLD);
            wattron(register_window, COLOR_PAIR(4));
            mvwprintw(register_window, i + 1, 2, " x%-3ld| %-4s | 0x%016lx ", i, register_aliases[i], vm->register_file->reg_arr[i]);
            wattroff(register_window, A_BOLD);
            wattroff(register_window, COLOR_PAIR(4));
        } else {
            mvwprintw(register_window, i + 1, 2, " x%-3ld| %-4s | 0x%016lx ", i, register_aliases[i], vm->register_file->reg_arr[i]);
        }
    }
    box(register_window, 0, 0);
    wattroff(register_window, COLOR_PAIR(1));
    wrefresh(register_window);
}

void draw_memory_window(vm_s* vm) {
    wclear(memory_window);
    wattron(memory_window, COLOR_PAIR(1));
    mvwprintw(memory_window, 1, 107, "X");
    mvwprintw(memory_window, 1, 2, "Memory");

    wattron(memory_window, A_BOLD);
    mvwprintw(memory_window, 3, 4, "Address |         Word         | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 ");
    mvwprintw(memory_window, 4, 4, "--------|----------------------|--------|--------|--------|--------|--------|--------|--------|--------");
    int j = 0;
    for (uint64_t i = mem_print_start; i <= vm->memory->size && j < 37; i+=8) {
        wattron(memory_window, COLOR_PAIR(3));
        mvwprintw(memory_window, j + 5, 4, 
            "0x%05lx ", 
            i);
        wattroff(memory_window, COLOR_PAIR(3));
        mvwprintw(memory_window, j + 5, 12, 
            "|  0x%016lx  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  |  0x%02x  ", 
            read_memory(vm->memory, i)
                     | read_memory(vm->memory, i + 1) << 8
                     | read_memory(vm->memory, i + 2) << 16
                     | read_memory(vm->memory, i + 3) << 24
                     | read_memory(vm->memory, i + 4) << 32
                     | read_memory(vm->memory, i + 5) << 40
                     | read_memory(vm->memory, i + 6) << 48
                     | read_memory(vm->memory, i + 7) << 56,
            vm->memory->data[i],
            vm->memory->data[i + 1],
            vm->memory->data[i + 2],
            vm->memory->data[i + 3],
            vm->memory->data[i + 4],
            vm->memory->data[i + 5],
            vm->memory->data[i + 6],
            vm->memory->data[i + 7]
        );
        j++;
    }
        wattroff(memory_window, A_BOLD);
    box(memory_window, 0, 0);
    wattroff(memory_window, COLOR_PAIR(1));
    wrefresh(memory_window);
}
