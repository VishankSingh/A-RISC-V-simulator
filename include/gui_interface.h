#include <stdint.h>
#include <ncurses.h>

#include "../include/vm.h"

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

#define BUFFER_SIZE 57

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

extern MEVENT event;


void gui_init();

void handle_resize();

void end_gui();

void draw_top_menu();

void draw_load_dialog(char* input, int input_index);

void draw_mem_show_dialog(char* input, int input_index);

void draw_mem_edit_dialog();

void draw_code_window(char* filename, vm_s* vm);

void draw_register_window(vm_s* vm);

void draw_memory_window(vm_s* vm);

void draw_error_dialog();
