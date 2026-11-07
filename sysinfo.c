static inline unsigned char sysinfo_inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

extern unsigned int heap_pos;

void print_hex(char *vid, int row, int col, unsigned int num, unsigned char color) {
    char hex[] = "0123456789ABCDEF";
    char buf[9];
    int i;
    for (i = 7; i >= 0; i--) {
        buf[i] = hex[num & 0xF];
        num >>= 4;
    }
    buf[8] = 0;
    int j = 0;
    while (buf[j] != 0) {
        vid[(row * 80 + col + j) * 2] = buf[j];
        vid[(row * 80 + col + j) * 2 + 1] = color;
        j++;
    }
}

void run_sysinfo() {
    char *vid = (char*) 0xb8000;
    int i;
    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }

    char *l1 = "================================";
    char *l2 = "        SYSTEM INFORMATION";
    char *l3 = "================================";
    char *l4 = "CPU Mode: 32-bit Protected Mode";
    char *l5 = "Architecture: x86";
    char *l6 = "Video Mode: VGA Text Mode (80x25)";
    char *l7 = "Memory Allocator: Bump Allocator (malloc)";
    char *l8 = "Heap Start Address: 0x00100000";

    int row = 3;
    int col = 20;

    char *lines[8] = {l1, l2, l3, l4, l5, l6, l7, l8};
    unsigned char colors[8] = {0x0e, 0x0f, 0x0e, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a};

    int k;
    for (k = 0; k < 8; k++) {
        int j = 0;
        while (lines[k][j] != 0) {
            vid[((row + k) * 80 + col + j) * 2] = lines[k][j];
            vid[((row + k) * 80 + col + j) * 2 + 1] = colors[k];
            j++;
        }
    }

    char *l9 = "Current Heap Pointer: 0x";
    int j = 0;
    while (l9[j] != 0) {
        vid[((row + 9) * 80 + col + j) * 2] = l9[j];
        vid[((row + 9) * 80 + col + j) * 2 + 1] = 0x0a;
        j++;
    }
    print_hex(vid, row + 9, col + j, heap_pos, 0x0a);

    char *l10 = "Press ESC to return to menu";
    j = 0;
    while (l10[j] != 0) {
        vid[((row + 12) * 80 + col + j) * 2] = l10[j];
        vid[((row + 12) * 80 + col + j) * 2 + 1] = 0x07;
        j++;
    }

    while (1) {
        if (sysinfo_inb(0x64) & 1) {
            unsigned char scancode = sysinfo_inb(0x60);
            if (scancode == 1) break;
        }
    }

    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }
}
