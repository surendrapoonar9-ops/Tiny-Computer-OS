char *video_memory = (char*) 0xb8000;
int cursor_pos = 80;

char input_buffer[256];
int buffer_index = 0;
int enter_pressed = 0;

char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','+','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0,' '
};

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void keyboard_handler_main() {
    unsigned char scancode = inb(0x60);

    if (scancode < 128) {
        char c = scancode_to_ascii[scancode];

        if (c == '\n') {
            enter_pressed = 1;
        } else if (c == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                cursor_pos--;
                video_memory[cursor_pos * 2] = ' ';
            }
        } else if (c) {
            video_memory[cursor_pos * 2] = c;
            video_memory[cursor_pos * 2 + 1] = 0x0f;
            cursor_pos++;
            input_buffer[buffer_index++] = c;
        }
    }

    outb(0x20, 0x20);
}
