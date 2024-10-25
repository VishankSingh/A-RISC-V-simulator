#include "../include/gui_utils.h"

WINDOW* init_window(int height, int width, int starty, int startx) {
    WINDOW* win;
    win = newwin(height, width, starty, startx);
    wnoutrefresh(win);
    return win;
}