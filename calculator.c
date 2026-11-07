extern char *video_memory;
extern char input_buffer[256];
extern int buffer_index;
extern int enter_pressed;
extern int cursor_pos;

int str_to_int(char *str, int len) {
    int result = 0;
    int i = 0;
    for (i = 0; i < len; i++) {
        result = result * 10 + (str[i] - '0');
    }
    return result;
}

void print_at(int row, char *msg) {
    char *vid = (char*) 0xb8000;
    int offset = row * 80 * 2;
    int i = 0;
    while (msg[i] != 0) {
        vid[offset + i * 2] = msg[i];
        vid[offset + i * 2 + 1] = 0x0f;
        i++;
    }
}

void print_num(int row, int col, int num) {
    char *vid = (char*) 0xb8000;
    int offset = (row * 80 + col) * 2;

    if (num == 0) {
        vid[offset] = '0';
        vid[offset + 1] = 0x0f;
        return;
    }

    char digits[12];
    int i = 0;
    int neg = 0;
    if (num < 0) { neg = 1; num = -num; }

    while (num > 0) {
        digits[i++] = (num % 10) + '0';
        num /= 10;
    }
    if (neg) digits[i++] = '-';

    int j;
    for (j = 0; j < i; j++) {
        vid[offset + j * 2] = digits[i - 1 - j];
        vid[offset + j * 2 + 1] = 0x0f;
    }
}

void check_calculator_input() {
    if (!enter_pressed) return;

    enter_pressed = 0;

    int op_pos = -1;
    char op = 0;
    int k;

    for (k = 0; k < buffer_index; k++) {
        if (input_buffer[k] == '+' || input_buffer[k] == '-' ||
            input_buffer[k] == '*' || input_buffer[k] == '/') {
            op = input_buffer[k];
            op_pos = k;
            break;
        }
    }

    if (op_pos == -1) {
        print_at(3, "Invalid format! Use: 5+3        ");
        buffer_index = 0;
        return;
    }

    int num1 = str_to_int(input_buffer, op_pos);
    int num2 = str_to_int(input_buffer + op_pos + 1, buffer_index - op_pos - 1);
    int result = 0;

    if (op == '+') result = num1 + num2;
    else if (op == '-') result = num1 - num2;
    else if (op == '*') result = num1 * num2;
    else if (op == '/') result = (num2 != 0) ? num1 / num2 : 0;

    print_at(3, "Result:              ");
    print_num(3, 8, result);

    char *vid = (char*) 0xb8000;
    int m;
    for (m = 0; m < 80; m++) {
        vid[(1 * 80 + m) * 2] = ' ';
        vid[(1 * 80 + m) * 2 + 1] = 0x0f;
    }

    buffer_index = 0;
    cursor_pos = 80;
}

static inline unsigned char calc_inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void draw_calc_taskbar(char *vid) {
    int i;
    for (i = 0; i < 80; i++) {
        vid[(24 * 80 + i) * 2] = ' ';
        vid[(24 * 80 + i) * 2 + 1] = 0x1f;
    }
    char *tb1 = "TINY COMPUTER";
    int j = 0;
    while (tb1[j] != 0) { vid[(24*80+1+j)*2] = tb1[j]; vid[(24*80+1+j)*2+1] = 0x1f; j++; }
    char *tb2 = "Calculator";
    j = 0;
    while (tb2[j] != 0) { vid[(24*80+22+j)*2] = tb2[j]; vid[(24*80+22+j)*2+1] = 0x1e; j++; }
    char *tb3 = "ESC: Back to Menu";
    j = 0;
    while (tb3[j] != 0) { vid[(24*80+60+j)*2] = tb3[j]; vid[(24*80+60+j)*2+1] = 0x1f; j++; }
}

void run_calculator() {
    char *vid = (char*) 0xb8000;
    int i;
    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }
    char *msg = "Calculator - Type: 5+3 then Enter (ESC to exit)";
    i = 0;
    while (msg[i] != 0) {
        vid[i * 2] = msg[i];
        vid[i * 2 + 1] = 0x0f;
        i++;
    }

    draw_calc_taskbar(vid);

    buffer_index = 0;
    enter_pressed = 0;
    cursor_pos = 80;

    char calc_scancode_to_ascii[128] = {
        0, 27, '1','2','3','4','5','6','7','8','9','0','-','+','\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
        0,'a','s','d','f','g','h','j','k','l',';','\'','`',
        0,'\\','z','x','c','v','b','n','m',',','.','/', 0,
        '*', 0,' '
    };

    while (1) {
        if (calc_inb(0x64) & 1) {
            unsigned char scancode = calc_inb(0x60);

            if (scancode == 1) break;

            if (scancode < 128) {
                char c = calc_scancode_to_ascii[scancode];

                if (c == '\n') {
                    enter_pressed = 1;
                } else if (c == '\b') {
                    if (buffer_index > 0) {
                        buffer_index--;
                        cursor_pos--;
                        vid[cursor_pos * 2] = ' ';
                    }
                } else if (c) {
                    vid[cursor_pos * 2] = c;
                    vid[cursor_pos * 2 + 1] = 0x0f;
                    cursor_pos++;
                    input_buffer[buffer_index++] = c;
                }
            }
        }
        check_calculator_input();
    }

    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }
}
