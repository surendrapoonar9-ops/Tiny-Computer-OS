static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

char editor_scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','+','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0,' '
};

char document[2000];
int doc_length = 0;

void run_text_editor() {
    char *vid = (char*) 0xb8000;

    // pehle keyboard buffer clean karo, purana leftover key na aaye
    while (inb(0x64) & 1) {
        inb(0x60);
    }

    int i;
    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }

    char *title = "Text Editor - Type below (ESC to exit)";
    i = 0;
    while (title[i] != 0) {
        vid[i * 2] = title[i];
        vid[i * 2 + 1] = 0x0f;
        i++;
    }

    int cursor = 80;
    doc_length = 0;

    while (1) {
        if (inb(0x64) & 1) {
            unsigned char scancode = inb(0x60);

            if (scancode == 1) {
                break;
            }

            if (scancode < 128) {
                char c = editor_scancode_to_ascii[scancode];

                if (c == '\b') {
                    if (doc_length > 0) {
                        doc_length--;
                        cursor--;
                        vid[cursor * 2] = ' ';
                    }
                } else if (c) {
                    if (c == '\n') {
                        cursor = ((cursor / 80) + 1) * 80;
                    } else {
                        vid[cursor * 2] = c;
                        vid[cursor * 2 + 1] = 0x0f;
                        cursor++;
                    }
                    document[doc_length++] = c;
                }
            }
        }
    }

    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }
}
