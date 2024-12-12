#include <stdint.h>
#include <ncurses.h>

#include "../include/vm.h"


extern _Bool is_file_loaded;
extern _Bool is_main;
extern _Bool is_exit;
extern _Bool is_input;
extern _Bool is_edit;
extern _Bool is_error_show;
extern _Bool is_mem_show;
extern _Bool is_mem_edit;
extern _Bool is_top_menu_load;
extern _Bool is_top_menu_mem_show;
extern _Bool is_top_menu_mem_edit;
extern _Bool is_error_show;
extern _Bool is_cache_details_show;

#define BUFFER_SIZE 57

#define temp_file "temp.txt"

extern FILE *code_file_;
extern uint64_t file_size;

extern char input_buffer[BUFFER_SIZE];
extern int input_index;
extern char input[BUFFER_SIZE];
extern uint64_t mem_start_addr;
extern uint64_t code_print_start;
extern uint64_t mem_print_start;

extern char error_msg[100];

extern WINDOW *top_menu;
extern WINDOW *load_dialog;
extern WINDOW *mem_show_dialog;
extern WINDOW *mem_edit_dialog;
extern WINDOW *error_dialog;
extern WINDOW *code_window;
extern WINDOW *register_window;
extern WINDOW *memory_window;
extern WINDOW *breakpoints_window;
extern WINDOW *stack_window;
extern WINDOW *cache_window;
extern WINDOW *cache_details_window;

extern WINDOW *art_window;

extern MEVENT event;


extern uint64_t cursor_x;
extern uint64_t cursor_y;



void gui_init();

void handle_resize();

void end_gui();

void draw_top_menu();

void draw_load_dialog(char* input, int input_index);

void draw_mem_show_dialog(char* input, int input_index);

void draw_mem_edit_dialog(char* input, int input_index);

void draw_code_window(char* filename, vm_s* vm);

void draw_register_window(vm_s* vm);

void draw_memory_window(vm_s* vm);

void draw_breakpoints_window(vm_s* vm);

void draw_stack_window(vm_s* vm);

void draw_cache_window(vm_s* vm);

void draw_cache_details_window(vm_s* vm);

void draw_art_window();

void draw_error_dialog();

void insert_newline();

void handle_backspace(FILE *code_file_);

void handle_space(FILE *code_file_);

void handle_left();

void handle_right();

void handle_up();

void handle_down();

void insert_char(FILE *code_file_, char ch);






