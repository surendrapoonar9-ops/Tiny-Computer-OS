static inline unsigned char about_inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void run_about() {
    char *vid = (char*) 0xb8000;
    int i;
    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }

    char *l1 = "================================";
    char *l2 = "        TINY COMPUTER OS";
    char *l3 = "================================";
    char *l4 = "Version: 1.0";
    char *l5 = "A custom operating system built from scratch";
    char *l6 = "using x86 Assembly and C.";
    char *l7 = "Features: Bootloader, Protected Mode, IDT,";
    char *l8 = "Interrupt-driven Keyboard, Memory Management";
    char *l9 = "Apps: Calculator, Text Editor, Snake Game";
    char *l10 = "Developed by: Surendra Poonar";
    char *l11 = "Project Exhibition - 2026";
    char *l12 = "Press ESC to return to menu";

    int row = 2;
    int col = 20;

    char *lines[12] = {l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12};
    unsigned char colors[12] = {0x0e, 0x0f, 0x0e, 0x0a, 0x0f, 0x0f, 0x0f, 0x0f, 0x0a, 0x0e, 0x0d, 0x07};

    int k;
    for (k = 0; k < 12; k++) {
        int j = 0;
        while (lines[k][j] != 0) {
            vid[((row + k) * 80 + col + j) * 2] = lines[k][j];
            vid[((row + k) * 80 + col + j) * 2 + 1] = colors[k];
            j++;
        }
    }

    while (1) {
        if (about_inb(0x64) & 1) {
            unsigned char scancode = about_inb(0x60);
            if (scancode == 1) break;
        }
    }

    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }
}
