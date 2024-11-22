/**
 * File Name: main.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include "../include/main.h"
#include "../include/call_stack.h"

#include "../include/globals.h"

#include "../include/gui_interface.h"
#include "../include/utils.h"

#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

#define CPU_DELAY 50000

vm_s* vm;


// CHANGES:
// vm.c:363     used uint64_t
// vm.c:374     used int64_t
// lexer.c:118  used long long instead of long
// lexer.c:125  used strtoull instead of strtol

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void* gui_handler() {
    while(1) {
        pthread_mutex_lock(&mutex);

        draw_top_menu();
        if (is_main) {
            draw_code_window(temp_file, vm);
            draw_register_window(vm);
            draw_breakpoints_window(vm);
            draw_stack_window(vm);
            draw_cache_window(vm);
            //draw_cache_details_window(vm);
            //draw_art_window();
        }
        if (is_top_menu_load) {
            draw_load_dialog(input_buffer, input_index);
        }
        else if (is_top_menu_mem_show) {
            draw_mem_show_dialog(input_buffer, input_index);
        }
        else if (is_mem_show) {
            draw_memory_window(vm);
        }
        else if (is_top_menu_mem_edit) {
            draw_mem_edit_dialog(input_buffer, input_index);
        }
        else if (is_error_show) {
            //draw_error_dialog(error_msg);
        }
        else if (is_cache_details_show) {
            draw_cache_details_window(vm);
        }
        pthread_mutex_unlock(&mutex);

        napms(100);
    }
    return NULL;
}

void* input_handler() {
    nodelay(stdscr, TRUE);

    while (1) {
        pthread_mutex_lock(&mutex);
        if (is_exit) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        int ch = getch();
        MEVENT event;
        if (ch == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                if (event.bstate == 2) /*click*/ {
                    if (event.y == 1 && event.x >= 1 && event.x <= 4) {
                        is_top_menu_load = 1;
                        is_top_menu_mem_show = 0;
                        is_top_menu_mem_edit = 0;
                        is_cache_details_show = 0;
                        is_input = 1;
                        curs_set(1);
                        refresh();
                        wrefresh(load_dialog);
                    } 
                    else if (event.y == 1 && event.x >= 9 && event.x <= 16) {
                        is_top_menu_load = 0;
                        is_top_menu_mem_show = 1;
                        is_top_menu_mem_edit = 0;
                        is_cache_details_show = 0;
                        is_input = 1;
                        curs_set(1);
                        refresh();
                    } 
                    else if (event.y == 1 && event.x >= 21 && event.x <= 28) {
                        is_top_menu_load = 0;
                        is_top_menu_mem_show = 0;
                        is_top_menu_mem_edit = 1;
                        is_cache_details_show = 0;
                        is_input = 1;
                        curs_set(1);
                        refresh();
                    } 
                    else if (event.y == 1 && event.x >= 33 && event.x <= 36) {
                        is_exit = 1;
                        remove(temp_file);
                        raise(SIGINT);
                    } 
                    else if (event.y == 1 && event.x >= 146 && event.x <= 157) {
                        toggle_cache(vm->cache, vm->filename);
                    }
                    else if (event.y == 1 && event.x >= 162 && event.x <= 174) {
                        is_top_menu_load = 0;
                        is_top_menu_mem_show = 0;
                        is_cache_details_show = 1;
                        refresh();
                    }
                    else if (event.y == 1 && event.x >= 179 && event.x <= 194) {
                        invalidate(vm->cache);
                    }
                    else if (event.y == 1 && event.x >= 199 && event.x <= 209) {
                        cache_dump(vm->cache, "cache_dump.txt");
                    }
                    else if (is_top_menu_load) {
                        if (event.x == 133 && event.y == 21) {
                            is_top_menu_load = 0;
                            is_input = 0;
                            memset(input_buffer, 0, BUFFER_SIZE);
                            input_index = 0;
                            curs_set(0);
                            refresh();
                            werase(load_dialog);
                            wrefresh(load_dialog);
                        }
                    } 
                    else if (is_top_menu_mem_show) {
                        if (event.x == 133 && event.y == 21) {
                            is_top_menu_mem_show = 0;
                            is_input = 0;
                            memset(input_buffer, 0, BUFFER_SIZE);
                            input_index = 0;
                            curs_set(0);
                            refresh();
                            werase(mem_show_dialog);
                            wrefresh(mem_show_dialog);
                        }
                    }
                    else if (is_top_menu_mem_edit) {
                        if (event.x == 133 && event.y == 21) {
                            is_top_menu_mem_edit = 0;
                            is_input = 0;
                            memset(input_buffer, 0, BUFFER_SIZE);
                            input_index = 0;
                            curs_set(0);
                            refresh();
                            werase(mem_edit_dialog);
                            wrefresh(mem_edit_dialog);
                        }
                    }
                    else if (is_mem_show) {
                        if (event.x == 160 && event.y == 8) {
                            is_mem_show = 0;
                            is_input = 0;
                            curs_set(0);
                            werase(memory_window);
                            wrefresh(memory_window);
                        }
                    }
                    else if (is_cache_details_show) {
                        if (event.x == 121 && event.y == 21) {
                            is_cache_details_show = 0;
                            curs_set(0);
                            werase(cache_details_window);
                            wrefresh(cache_details_window);
                        }
                    }
                    
                    else if (is_main) {
                        if (!is_edit) {
                            if (event.y <= 53 && event.y >= 6 && event.x >= 1 && event.x <= 8) {     
                                int line_no = (event.y - 5) + code_print_start;
                                int instruction_no = get_instnum_linenum(vm->input->lil_map, line_no);
                                if (instruction_no != -1) {
                                    uint64_t address = 0x00000 + 4 * (instruction_no - 1);
                                    toggle_breakpoint(vm, address);                
                                }

                            }
                            else if (event.y == 4 && event.x >= 122 && event.x <= 138) {
                                curs_set(1);
                                is_edit = 1;
                            }
                            else if (event.y == 21 && event.x >= 142 && event.x <= 151) {
                                for (int i = 1; i < MAX_BREAKPOINTS; i++) {
                                    vm->breakpoints[i] = INT64_MAX;
                                }
                            }
                        } else if (is_edit) {
                            if (event.y == 4 && event.x >= 132 && event.x <= 138) {
                                is_edit = 0;
                                curs_set(0);
                                FILE *source = fopen(vm->filename, "r");
                                FILE *dest = fopen(temp_file, "w");
                                char c;
                                while ((c = fgetc(source)) != EOF) {
                                    fputc(c, dest);
                                }
                                fclose(source);
                                fclose(dest);
                                file_size = count_lines_in_file(temp_file);
                            } 
                            else if (event.y == 4 && event.x >= 120 && event.x <= 130) {
                                is_edit = 0;
                                curs_set(0);
                                FILE *source = fopen(temp_file, "r");
                                FILE *dest = fopen(vm->filename, "w");

                                char c;
                                while ((c = fgetc(source)) != EOF) {
                                    fputc(c, dest);
                                }
                                    

                                while ((c = fgetc(source)) != EOF) {
                                    fputc(c, dest);
                                }
                                fclose(source);
                                fclose(dest);
                                vm_reset(vm, vm->filename);
                                if (!is_file_empty(vm->filename)) {
                                    assemble(output_file_, vm);
                                }
                                load_binary_instructions(vm->memory, vm->input->binary, vm->input->binary_index);
                                vm->breakpoints[0] = 0x00000 + 4 * vm->input->binary_index;
                                file_size = count_lines_in_file(vm->filename);
                                int line_num = get_linenum_instnum(vm->input->lil_map, 1) - 1;
                                lil_node_s node = {0, line_num, "main"};
                                push(vm->call_stack, &node);
                            }
                        }
                    }
                    else {
                        
                    }


                }
                else if (event.bstate == 65536 && !is_edit && !is_input) {
                    if (is_top_menu_load) {
                    } 
                    else if (is_mem_show) {
                        if (event.x > 70 && event.x < 142 && event.y > 6 && event.y < 48) {
                            if (mem_print_start > 0x00000 + 24) {
                                mem_print_start -= 24;
                            } else {
                                mem_print_start = 0;
                            }
                        }
                    }
                    
                    
                    else if (is_main) {
                        if (event.x > 0 && event.x < 2 * COLS / 3 && event.y > 3 && event.y < LINES - 3) {
                            if (code_print_start > 11) {
                                code_print_start -= 10;
                            } else {
                                code_print_start = 0;
                            }
                        }
                    }
                }
                else if (event.bstate == 2097152 && !is_edit && !is_input) {
                    if (is_top_menu_load) {
                    } 
                    else if (is_mem_show) {
                        if (event.x > 70 && event.x < 142 && event.y > 6 && event.y < 48) {
                            if (mem_print_start < 0x50000 - 24) {
                                mem_print_start += 24;
                            } else {
                                mem_print_start = 0x50000;
                            }
                        }
                    }
                    
                    else {
                        if (event.x > 0 && event.x < 2 * COLS / 3 && event.y > 3 && event.y < LINES - 3) {
                            if (vm->input->size > (unsigned int)(4 + LINES - 11)) {

                                if (code_print_start + 4 + LINES - 11 < vm->input->size) {
                                    code_print_start += 10;
                                } else {
                                    code_print_start = vm->input->size - 4 - LINES + 11;
                                }
                            }
                        }
                    }
                    
                }
                else {
                    //wclear(top_menu);
                    //mvwprintw(top_menu, 1, 2, "Other mouse event: (%d)", event.bstate);
                    //wrefresh(top_menu);
                }
            }
        } 
        else if (ch != ERR) {
            if (is_input) {
                if (ch == KEY_ENTER || ch == '\n') {
                    input_buffer[input_index] = '\0';
                    input_index = 0;
                    is_input = 0;
                    memcpy(input, input_buffer, BUFFER_SIZE);
                    memset(input_buffer, 0, BUFFER_SIZE);
                    if (is_top_menu_load) {
                        if (input[0] != '\0') {
                            
                            if (is_regular_file(input)) {
                                vm_reset(vm, input);
                                if (!is_file_empty(input)) {
                                    assemble(output_file_, vm);
                                }
                                
                                
                                //assemble(output_file_, vm);
                                load_binary_instructions(vm->memory, vm->input->binary, vm->input->binary_index);
                                vm->breakpoints[0] = 0x00000 + 4 * vm->input->binary_index;
                                
                                FILE *source = fopen(input, "r");
                                FILE *dest = fopen(temp_file, "w");
                                char c;
                                while ((c = fgetc(source)) != EOF) {
                                    fputc(c, dest);
                                }
                                fclose(source);
                                fclose(dest);

                                file_size = count_lines_in_file(temp_file);
                                is_file_loaded = 1;
                                curs_set(0);

                                if (error_code_) {
                                    continue;
                                }
                                int line_num = get_linenum_instnum(vm->input->lil_map, 1) - 1;
                                lil_node_s node = {0, line_num, "main"};
                                push(vm->call_stack, &node);
                                code_print_start = 0;
                                cursor_x = 0;
                                cursor_y = 0;
                                
                            }
                        }
                        is_top_menu_load = 0;
                        is_input = 0;
                        input_index = 0;
                        //curs_set(0);
                        werase(load_dialog);
                        wrefresh(load_dialog);
                    } 
                    else if (is_top_menu_mem_show) {
                        if (input[0] != '\0') {
                            if (is_valid_hexadecimal(input)) {
                                mem_print_start = strtol(input, NULL, 16);
                                is_mem_show = 1;
                            }
                        }
                        is_top_menu_mem_show = 0;
                        is_input = 0;
                        input_index = 0;
                        curs_set(0);
                        werase(mem_show_dialog);
                        wrefresh(mem_show_dialog);
                    }
                    else if (is_top_menu_mem_edit) {
                        char *token = malloc(BUFFER_SIZE * sizeof(char));
                        memcpy(token, input, BUFFER_SIZE);
                        memset(input, 0, BUFFER_SIZE);
                        token = strtok(token, " ");
                        if (token != NULL) {
                            uint64_t address = strtol(token, NULL, 16);
                            token = strtok(NULL, " ");
                            if (address > 0 && address <= MAX_MEMORY_SIZE && token != NULL) {
                                uint64_t data = strtol(token, NULL, 16);
                                token = strtok(NULL, " ");
                                uint64_t bytes = strtol(token, NULL, 10);
                                write_memory(vm->memory, address, data, bytes);
                            }
                        }
                    }
                }
                else if (ch == KEY_BACKSPACE) {
                    if (input_index > 0) {
                        input_index--;
                        input_buffer[input_index] = '\0';
                        if (is_top_menu_load) {
                        }
                    }
                }
                else if (ch == KEY_LEFT) {
                    if (input_index > 0) {
                        input_index--;
                        if (is_top_menu_load) {
                        }
                    }
                    
                    
                }
                else if (ch == KEY_RIGHT) {
                    if ((size_t)input_index < strlen(input_buffer) - 1) {
                        input_index++;
                        if (is_top_menu_load) {
                        }
                    }
                }
                else if (ch >= 32 && ch <= 126) {
                    if (input_index < BUFFER_SIZE - 1) {
                        input_buffer[input_index++] = ch;
                        if (is_top_menu_load) {
                        }
                    }
                }
            }
            else if (is_edit) {
                if (ch == KEY_ENTER || ch == '\n') {
                    insert_newline();
                }
                else if (ch == KEY_BACKSPACE) {
                    FILE *file = fopen(temp_file, "r+");
                    handle_backspace(file);
                    fclose(file);
                    
                }
                else if (ch == ' ') {
                    FILE *file = fopen(temp_file, "r+");
                    handle_space(file);
                    fclose(file);
                }
                else if (ch == KEY_LEFT) {
                    if (cursor_x > 0) {
                        cursor_x--;
                    }
                }
                else if (ch == KEY_RIGHT) {
                    handle_right();
                }
                else if (ch == KEY_UP) {
                    if (cursor_y > 0) {
                        cursor_y--;
                        if (code_print_start > 0 && cursor_y < code_print_start) {
                            code_print_start = cursor_y;
                        } else if (cursor_y > code_print_start + LINES - 8) {
                            code_print_start = cursor_y;
                        }
                    }
                }
                else if (ch == KEY_DOWN) {
                    if (cursor_y < file_size) {
                        cursor_y++;
                        if (cursor_y > code_print_start + LINES - 8) {
                            code_print_start = cursor_y - LINES + 8;
                        } else if (cursor_y == code_print_start + LINES - 8) {
                            code_print_start = cursor_y - LINES + 7;
                        }
                        
                         else if (cursor_y < code_print_start) {
                            code_print_start = cursor_y;
                        }
                    }
                }
                else if (ch >= 32 && ch <= 126) {
                    FILE *file = fopen(temp_file, "r+");
                    insert_char(file, ch);
                    fclose(file);
                }


            }
            
            else if (ch == 'q') {
                is_exit = 1;
                remove(temp_file);
                raise(SIGINT);
            } 
            else if (ch == 'l') {
                if (is_top_menu_mem_show == 0 && is_mem_show == 0 && is_input == 0) {
                    is_top_menu_mem_show = 1;
                    is_input = 1;
                    curs_set(1);
                    refresh();
                }
            } 
            else if (ch == 'm') {
                if (is_top_menu_load == 0 && is_mem_show == 0 && is_input == 0) {
                    is_top_menu_mem_show = 1;
                    is_input = 1;
                    curs_set(1);
                    refresh();
                }
            }
            else if (ch == 'r') {
                if (is_regular_file(vm->filename) && !error_code_ && !(is_edit) && !is_input && !is_top_menu_load && !is_top_menu_mem_show && !is_top_menu_mem_edit) {
                    vm_reset(vm, vm->filename);
                    assemble(output_file_, vm);
                    load_binary_instructions(vm->memory, vm->input->binary, vm->input->binary_index);
                    vm->breakpoints[0] = 0x00000 + 4 * vm->input->binary_index;
                    int line_num = get_linenum_instnum(vm->input->lil_map, 1) - 1;
                    lil_node_s node = {0, line_num, "main"};
                    push(vm->call_stack, &node);
                }
                
            } 
            else if (ch == 'w' || ch == ' ') {
                if (!error_code_ && !(is_edit) && !is_input && !is_top_menu_load && !is_top_menu_mem_show && !is_top_menu_mem_edit) {
                    vm_run(vm, 1);
                }
            }
            else if (ch == KEY_UP) {
                
            } 
            else if (ch == KEY_DOWN) {
                
            } 
            else if (ch == KEY_RIGHT || ch == 's') {
                if (!error_code_ && strcmp(vm->filename, "") != 0 && !(is_edit) && !is_input && !is_top_menu_load && !is_top_menu_mem_show && !is_top_menu_mem_edit) {
                    vm_run(vm, 0);
                }
            }
           
        
        }
        pthread_mutex_unlock(&mutex);
        napms(0);
        // Add any other processing or updates here
    }
    return NULL;
}


int main(void) {
    pthread_t tid1;
    pthread_t tid2;
    vm = init_vm_s(NULL);
#ifdef lol
    vm_s *vm = init_vm_s(NULL);
    char command_buffer[100];
    char *command = (char *)malloc(100 * sizeof(char));
    char *token = (char *)malloc(100 * sizeof(char));
    while (1) {
        //printf(MAG BOLD"=> "RESET);
        fgets(command_buffer, sizeof(command_buffer), stdin); 
        if (command_buffer[0] == '\n') {
            continue;
        }
        command_buffer[strcspn(command_buffer, "\n")] = '\0';
        command = strtok(command_buffer, " ");
        if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0 || strcmp(command, "q") == 0) {
            printf("Exited the simulator\n");
            break;
        } 
        else if (strcmp(command, "load") == 0 || strcmp(command, "ld") == 0) {
            char *input_file = strtok(NULL, " ");
            if (input_file == NULL) {
                printf("Invalid command\n\n");
                continue;
            }
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command\n\n");
                continue;
            }
            input_file[strcspn(input_file, "\n")] = '\0';
            FILE *file = fopen(input_file, "r");
            if (file == NULL) {
                printf("File not found\n\n");
                continue;
            }
            vm_reset(vm, input_file);
            assemble(output_file_, vm);
            if (error_code_) {
                printf("\n");
                continue;
            }
            int line_num = get_linenum_instnum(vm->input->lil_map, 1) - 1;
            lil_node_s node = {0, line_num, "main"};
            push(vm->call_stack, &node);
            
            load_binary_instructions(vm->memory, vm->input->binary, vm->input->binary_index);
            add_breakpoint(vm, 0x00000 + 4 * vm->input->binary_index);


            printf("\n");
        } 
        else if (strcmp(command, "run") == 0 || strcmp(command, "r") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            if (strcmp(vm->filename, "") == 0) {
                printf("Load file first\n\n");
                continue;
            }
            if (error_code_) {
                printf("File has errors\n\n");
                continue;
            }
            vm_run(vm, 1);
            printf("\n");
        } 
        else if (strcmp(command, "step") == 0 || strcmp(command, "s") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            if (strcmp(vm->filename, "") == 0) {
                printf("Load file first\n\n");
                continue;
            }
            if (error_code_) {
                printf("File has errors\n\n");
                continue;
            }
            vm_run(vm, 0);
            printf("\n");
        } 
        else if (strcmp(command, "regs") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            print_registers(vm->register_file);
            printf("\n");
        } 
        else if (strcmp(command, "mem") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            int64_t address = strtol(token, NULL, 16);
            if (address < 0 || address >= MAX_MEMORY_SIZE) {
                printf("Invalid address\n\n");
                continue;
            }
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format\n\n");
                continue;
            }

            uint64_t size = strtol(token, NULL, 10);
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            if (size <= 0 || size > MAX_MEMORY_SIZE) {
                printf("Invalid size\n\n");
                continue;
            }

            print_memory(vm->memory, address, size);
            printf("\n");
        } 
        else if (strcmp(command, "break") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            int line_no = strtol(token, NULL, 10);
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            int instruction_no = get_instnum_linenum(vm->input->lil_map, line_no);
            if (instruction_no == -1) {
                printf("Invalid line number\n\n");
                continue;
            }
            if (strcmp(vm->filename, "") == 0) {
                printf("Load file first\n\n");
                continue;
            }
            uint64_t address = 0x00000 + 4 * (instruction_no - 1);
            add_breakpoint(vm, address);
            printf("Breakpoint set at line %d\n", line_no);
            printf("\n");
        } 
        else if (strcmp(command, "del-break") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            int line_no = strtol(token, NULL, 10);
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            int instruction_no = get_instnum_linenum(vm->input->lil_map, line_no);
            uint64_t address = 0x00000 + 4 * (instruction_no - 1);
            remove_breakpoint(vm, address);
        } 
        else if (strcmp(command, "breakpoints") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            for (int i = 1; i < MAX_BREAKPOINTS; i++) {
                if (vm->breakpoints[i] != INT64_MAX) {
                    int line_no = get_linenum_instnum(vm->input->lil_map, vm->breakpoints[i] / 4 + 1);
                    printf("%d\n", line_no);
                }
            }
            printf("\n");
        } 
        else if (strcmp(command, "show-stack") == 0 || strcmp(command, "stack") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            if (is_empty(vm->call_stack)) {
                printf("empty call stack: execution complete\n");
                printf("\n");
                continue;
            }
            int64_t curr_line;

            if (vm->last_pc == 0x00000 - 4) {
                curr_line = 0;
            } else {
                curr_line = get_linenum_instnum(vm->input->lil_map, vm->last_pc / 4 + 1);
            }

            if (curr_line == -1) {
                printf("Stack in Empty\n");
                printf("\n");
                continue;
            }
            printf("Call Stack:\n");
            print_stack(vm->call_stack);
            printf("\n");
        } 
        else if (strcmp(command, "pc") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            printf("PC: %05lx\n", vm->pc);
        } 
        else if (strcmp(command, "input") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            if (strcmp(vm->filename, "") != 0) {
                printf("%s\n", vm->filename);
                for (unsigned int i = 0; i < vm->input->size; i++) {
                    printf("%-3d| %s\n", i + 1, &vm->input->file[i * 1024]);
                }

            }
            else
                printf("Load file first\n");
            printf("\n");
        } 
        else if (strcmp(command, "reset") == 0 || strcmp(command, "rst") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            if (strcmp(vm->filename, "") == 0) {
                printf("\n");
                continue;
            }
            vm_reset(vm, vm->filename);
            assemble(output_file_, vm);
            load_binary_instructions(vm->memory, vm->input->binary, vm->input->binary_index);
            add_breakpoint(vm, 0x00000 + 4 * vm->input->binary_index);
            printf("\n");
        } 
        else if (strcmp(command, "mem_write") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            int64_t address = strtol(token, NULL, 16);
            if (address < 0 || address >= MAX_MEMORY_SIZE) {
                printf("Invalid address\n\n");
                continue;
            }
            if (address % 4 != 0) {
                printf("Address must be a multiple of 4\n\n");
                continue;
            }
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            int64_t data = strtol(token, NULL, 16);
            if (data < 0 || data >= 0x100) {
                printf("Invalid data\n\n");
                continue;
            }
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            int bytes = strtol(token, NULL, 10);
            write_memory(vm->memory, address, data, bytes);
        } 
        else if (strcmp(command, "help") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                printf("Invalid command format\n\n");
                continue;
            }
            printf("Commands:\n");
            printf("load <filename> - Load the file\n");
            printf("run - Run the program\n");
            printf("step - Step through the program\n");
            printf("regs - Print the register values\n");
            printf("mem <address> <size> - Print memory contents\n");
            printf("break <line number> - Set a breakpoint\n");
            printf("del-break <line number> - Delete a breakpoint\n");
            printf("breakpoints - Print all breakpoints\n");
            printf("show-stack/stack - Print the call stack\n");
            printf("pc - Print the program counter\n");
            printf("input - Print the input file\n");
            printf("reset - Reset the simulator\n");
            printf("mem_write <address> <data> <bytes> - Write data to memory\n");
            printf("exit - Exit the simulator\n");
            printf("\n");
        } else {
            printf("Invalid command\n");
            printf("Type 'help' for list of commands\n");
            printf("\n");
        } 
    }
    free(token);
    free_vm_s(vm);

#endif
    gui_init();

    pthread_create(&tid1, NULL, gui_handler, NULL);
    pthread_create(&tid2, NULL, input_handler, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    //end_gui();
    pthread_mutex_destroy(&mutex);


    //free_vm_s(vm);


    return EXIT_SUCCESS;
}
