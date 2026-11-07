#include "idt.h"
#include "memory.h"
#include "calculator.h"
#include "texteditor.h"
#include "snake.h"
#include "about.h"
#include "sysinfo.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void flush_keyboard() {
    while (inb(0x64) & 1) {
        inb(0x60);
    }
}

void clear_screen(char *vid) {
    int i;
    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }
}

void menu_put_char(char *vid, int row, int col, char ch, unsigned char color) {
    vid[(row * 80 + col) * 2] = ch;
    vid[(row * 80 + col) * 2 + 1] = color;
}

void menu_print_at(char *vid, int row, int col, char *msg, unsigned char color) {
    int i = 0;
    while (msg[i] != 0) {
        menu_put_char(vid, row, col + i, msg[i], color);
        i++;
    }
}

void draw_box(char *vid, int row1, int col1, int row2, int col2, unsigned char color) {
    int r, c;
    menu_put_char(vid, row1, col1, 218, color);
    menu_put_char(vid, row1, col2, 191, color);
    menu_put_char(vid, row2, col1, 192, color);
    menu_put_char(vid, row2, col2, 217, color);

    for (c = col1 + 1; c < col2; c++) {
        menu_put_char(vid, row1, c, 196, color);
        menu_put_char(vid, row2, c, 196, color);
    }
    for (r = row1 + 1; r < row2; r++) {
        menu_put_char(vid, r, col1, 179, color);
        menu_put_char(vid, r, col2, 179, color);
    }
}

void draw_taskbar(char *vid, char *app_name) {
    int i;
    for (i = 0; i < 80; i++) {
        menu_put_char(vid, 24, i, ' ', 0x1f);
    }
    menu_print_at(vid, 24, 1, "TINY COMPUTER", 0x1f);
    menu_print_at(vid, 24, 20, "|", 0x1f);
    menu_print_at(vid, 24, 22, app_name, 0x1e);
    menu_print_at(vid, 24, 60, "ESC: Back to Menu", 0x1f);
}

void show_menu() {
    char *vid = (char*) 0xb8000;
    clear_screen(vid);

    draw_box(vid, 1, 10, 22, 69, 0x0b);

    menu_print_at(vid, 2, 27, "WELCOME TO", 0x0e);
    menu_print_at(vid, 3, 22, "* TINY COMPUTER *", 0x0f);

    draw_box(vid, 5, 20, 7, 59, 0x0a);
    menu_print_at(vid, 6, 24, "[ 1 ]  Calculator", 0x0a);

    draw_box(vid, 8, 20, 10, 59, 0x0d);
    menu_print_at(vid, 9, 24, "[ 2 ]  Text Editor", 0x0d);

    draw_box(vid, 11, 20, 13, 59, 0x0c);
    menu_print_at(vid, 12, 24, "[ 3 ]  Snake Game", 0x0c);

    draw_box(vid, 14, 20, 16, 59, 0x0b);
    menu_print_at(vid, 15, 24, "[ 4 ]  System Info", 0x0b);

    draw_box(vid, 17, 20, 19, 59, 0x0e);
    menu_print_at(vid, 18, 24, "[ 5 ]  About", 0x0e);

    menu_print_at(vid, 20, 24, "Press 1-5 to select", 0x0f);

    draw_taskbar(vid, "Home Menu");
}

void kernel_main() {
    while (1) {
        flush_keyboard();
        show_menu();

        int choice = 0;
        while (choice == 0) {
            if (inb(0x64) & 1) {
                unsigned char scancode = inb(0x60);
                if (scancode == 2) choice = 1;
                else if (scancode == 3) choice = 2;
                else if (scancode == 4) choice = 3;
                else if (scancode == 5) choice = 4;
                else if (scancode == 6) choice = 5;
            }
        }

        flush_keyboard();

        if (choice == 1) run_calculator();
        else if (choice == 2) run_text_editor();
        else if (choice == 3) run_snake_game();
        else if (choice == 4) run_sysinfo();
        else if (choice == 5) run_about();
    }
}
