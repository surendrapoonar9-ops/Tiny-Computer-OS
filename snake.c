static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void io_wait() {
    volatile int i;
    for (i = 0; i < 28000000; i++) { }
}

#define OFFSET_Y 2
#define WIDTH 78
#define HEIGHT 20
#define MAX_LEN 200

int snake_x[MAX_LEN];
int snake_y[MAX_LEN];
int snake_len;
int dir_x, dir_y;
int food_x, food_y;
int game_over;
int quit_game;

void draw_snake() {
    char *vid = (char*) 0xb8000;

    int i;
    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }

    int x, y;
    for (x = 0; x <= WIDTH; x++) {
        vid[((OFFSET_Y) * 80 + x) * 2] = '-';
        vid[((OFFSET_Y) * 80 + x) * 2 + 1] = 0x0e;
        vid[((OFFSET_Y + HEIGHT) * 80 + x) * 2] = '-';
        vid[((OFFSET_Y + HEIGHT) * 80 + x) * 2 + 1] = 0x0e;
    }
    for (y = 0; y <= HEIGHT; y++) {
        vid[((OFFSET_Y + y) * 80 + 0) * 2] = '|';
        vid[((OFFSET_Y + y) * 80 + 0) * 2 + 1] = 0x0e;
        vid[((OFFSET_Y + y) * 80 + WIDTH) * 2] = '|';
        vid[((OFFSET_Y + y) * 80 + WIDTH) * 2 + 1] = 0x0e;
    }

    for (i = 0; i < snake_len; i++) {
        int pos = ((OFFSET_Y + snake_y[i]) * 80) + snake_x[i];
        vid[pos * 2] = '#';
        vid[pos * 2 + 1] = 0x0a;
    }

    int fpos = ((OFFSET_Y + food_y) * 80) + food_x;
    vid[fpos * 2] = '*';
    vid[fpos * 2 + 1] = 0x0c;
}

unsigned int rand_seed = 12345;
int simple_rand() {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed / 65536) % 32768;
}

void place_food() {
    food_x = 1 + (simple_rand() % (WIDTH - 2));
    food_y = 1 + (simple_rand() % (HEIGHT - 2));
}

void play_one_round() {
    while (inb(0x64) & 1) { inb(0x60); }
    snake_len = 3;
    snake_x[0] = 20; snake_y[0] = 10;
    snake_x[1] = 19; snake_y[1] = 10;
    snake_x[2] = 18; snake_y[2] = 10;
    dir_x = 1; dir_y = 0;
    game_over = 0;

    place_food();
    draw_snake();

    while (!game_over) {
        if (inb(0x64) & 1) {
            unsigned char scancode = inb(0x60);

            if (scancode == 1) {
                game_over = 1;
                quit_game = 1;
            }
            if (scancode == 17 && dir_y != 1) { dir_x = 0; dir_y = -1; }
            if (scancode == 31 && dir_y != -1) { dir_x = 0; dir_y = 1; }
            if (scancode == 30 && dir_x != 1) { dir_x = -1; dir_y = 0; }
            if (scancode == 32 && dir_x != -1) { dir_x = 1; dir_y = 0; }
        }

        if (game_over) break;

        int new_x = snake_x[0] + dir_x;
        int new_y = snake_y[0] + dir_y;

        if (new_x < 1 || new_x > WIDTH - 1 || new_y < 1 || new_y > HEIGHT - 1) {
            game_over = 1;
            break;
        }

        int i;
        for (i = 0; i < snake_len; i++) {
            if (snake_x[i] == new_x && snake_y[i] == new_y) {
                game_over = 1;
                break;
            }
        }
        if (game_over) break;

        for (i = snake_len - 1; i > 0; i--) {
            snake_x[i] = snake_x[i - 1];
            snake_y[i] = snake_y[i - 1];
        }
        snake_x[0] = new_x;
        snake_y[0] = new_y;

        if (new_x == food_x && new_y == food_y) {
            if (snake_len < MAX_LEN) snake_len++;
            place_food();
        }

        draw_snake();

        io_wait();
    }
}

void run_snake_game() {
    quit_game = 0;

    while (!quit_game) {
        play_one_round();

        if (quit_game) break;

        char *vid = (char*) 0xb8000;
        int i;
        for (i = 0; i < 80 * 25; i++) {
            vid[i * 2] = ' ';
            vid[i * 2 + 1] = 0x0f;
        }
        char *msg = "Game Over! Press R to restart, ESC to exit.";
        for (i = 0; msg[i] != 0; i++) {
            vid[i * 2] = msg[i];
            vid[i * 2 + 1] = 0x0f;
        }

        while (1) {
            if (inb(0x64) & 1) {
                unsigned char scancode = inb(0x60);
                if (scancode == 1) {
                    quit_game = 1;
                    break;
                }
                if (scancode == 19) {
                    break;
                }
            }
        }
    }

    char *vid = (char*) 0xb8000;
    int i;
    for (i = 0; i < 80 * 25; i++) {
        vid[i * 2] = ' ';
        vid[i * 2 + 1] = 0x0f;
    }
}
