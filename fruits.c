// fruits.c
//gcc fruits.c -o fruits.exe
//.\fruits.exe
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WIDTH 60
#define HEIGHT 20
#define MAX_OBJECTS 15
#define TARGET_FPS 20
#define FRAME_TIME_MS (1000 / TARGET_FPS)

#define TYPE_FRUIT 0
#define TYPE_BOMB  1

typedef struct {
    int x, y;
    int dx;
    char symbol;
    int active;
    int type;
} GameObject;

/* ------------------ GLOBAL STATE ------------------ */
static GameObject objects[MAX_OBJECTS];
static char screen[HEIGHT][WIDTH];
static char oldScreen[HEIGHT][WIDTH];
static int blade_x, blade_y;
static int score, quit, game_over;
static int fruit_update_counter, oldScreen_initialized;

static int current_level;
static int fruit_spawn_chance, bomb_spawn_ratio, fruit_fall_delay;

/* ------------------ CONSOLE ------------------ */
static void gotoxy(int x, int y) {
    static HANDLE h = NULL;
    if (!h) h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(h, c);
}

static void hideCursor(void) {
    CONSOLE_CURSOR_INFO cci = {1, FALSE};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
}

static void lockConsoleSize(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD buf = { WIDTH, HEIGHT + 3 };
    SMALL_RECT win = { 0, 0, WIDTH - 1, HEIGHT + 2 };
    SetConsoleScreenBufferSize(h, buf);
    SetConsoleWindowInfo(h, TRUE, &win);
}

/* -------- KEY STATE FLUSH (CRITICAL FIX) -------- */
static void waitForKeyRelease(void) {
    while (GetAsyncKeyState(VK_RETURN) & 0x8000 ||
           GetAsyncKeyState(VK_ESCAPE) & 0x8000 ||
           GetAsyncKeyState(VK_UP) & 0x8000 ||
           GetAsyncKeyState(VK_DOWN) & 0x8000) {
        Sleep(10);
    }
}

/* ------------------ BORDER ------------------ */
static void drawStaticBorder(void) {
    for (int x = 0; x < WIDTH; x++)
        screen[0][x] = screen[HEIGHT - 1][x] = '#';
    for (int y = 0; y < HEIGHT; y++)
        screen[y][0] = screen[y][WIDTH - 1] = '#';
}

static void clearInnerArea(void) {
    for (int y = 1; y < HEIGHT - 1; y++)
        for (int x = 1; x < WIDTH - 1; x++)
            screen[y][x] = ' ';
}

/* ------------------ TITLE ------------------ */
static void printTitle(void) {
    printf("FFFFFFFF   RRRRRR     U     U  IIIII  TTTTTTT      N     N  IIIII  N      N   JJJJJJJ  AAAAAAA\n");
    printf("FF         RR    R    U     U    I       T         NN    N    I    NN     N      JJ    A     A\n");
    printf("FF         RR    R    U     U    I       T         N N   N    I    N  N   N      JJ    A     A\n");
    printf("FFFFFF     RRRRRR     U     U    I       T         N  N  N    I    N   N  N      JJ    AAAAAAA\n");
    printf("FF         RR  RR     U     U    I       T         N   N N    I    N    N N  J   JJ    A     A\n");
    printf("FF         RR   RR     U   U     I       T         N    NN    I    N     NN  J   JJ    A     A\n");
    printf("FF         RR    RR     UUU    IIIII     T         N     N  IIIII  N      N  JJJJ      A     A\n");
}

/* ------------------ LEVEL CONFIG ------------------ */
static void configureLevel(int level) {
    current_level = level;
    if (level == 1) { fruit_spawn_chance = 12; bomb_spawn_ratio = 5; fruit_fall_delay = 5; }
    else if (level == 2) { fruit_spawn_chance = 9; bomb_spawn_ratio = 4; fruit_fall_delay = 3; }
    else { fruit_spawn_chance = 8; bomb_spawn_ratio = 3; fruit_fall_delay = 2; }
}

/* ------------------ RESET GAME ------------------ */
static void resetGameState(void) {
    memset(objects, 0, sizeof(objects));
    memset(screen, ' ', sizeof(screen));
    memset(oldScreen, 0, sizeof(oldScreen));

    score = quit = game_over = fruit_update_counter = oldScreen_initialized = 0;
    blade_x = WIDTH / 2;
    blade_y = HEIGHT - 2;

    drawStaticBorder();
}

/* ------------------ SPAWN ------------------ */
static void spawnObject(void) {
    if (rand() % fruit_spawn_chance) return;

    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (!objects[i].active) {
            objects[i].x = 1 + rand() % (WIDTH - 2);
            objects[i].y = 1;
            objects[i].dx = (rand() % 2) ? 1 : -1;
            objects[i].active = 1;
            if (rand() % bomb_spawn_ratio == 0) {
                objects[i].symbol = 'X'; objects[i].type = TYPE_BOMB;
            } else {
                objects[i].symbol = "*o+#@"[rand() % 5]; objects[i].type = TYPE_FRUIT;
            }
            return;
        }
    }
}

/* ------------------ UPDATE ------------------ */
static void update(void) {
    if (GetAsyncKeyState('Q') & 0x8000 || GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        quit = 1; return;
    }

    if (game_over) return;

    if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && blade_x > 1) blade_x--;
    if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && blade_x < WIDTH - 2) blade_x++;
    if ((GetAsyncKeyState(VK_UP) & 0x8000) && blade_y > 1) blade_y--;
    if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && blade_y < HEIGHT - 2) blade_y++;

    if (++fruit_update_counter >= fruit_fall_delay) {
        fruit_update_counter = 0;
        for (int i = 0; i < MAX_OBJECTS; i++) {
            if (!objects[i].active) continue;
            if (current_level == 3) {
                objects[i].x += objects[i].dx;
                if (objects[i].x <= 1 || objects[i].x >= WIDTH - 2)
                    objects[i].dx *= -1;
            }
            if (++objects[i].y >= HEIGHT - 1) objects[i].active = 0;
        }
        spawnObject();
    }

    clearInnerArea();
    for (int i = 0; i < MAX_OBJECTS; i++)
        if (objects[i].active)
            screen[objects[i].y][objects[i].x] = objects[i].symbol;

    screen[blade_y][blade_x] = '|';

    for (int i = 0; i < MAX_OBJECTS; i++)
        if (objects[i].active && objects[i].x == blade_x && objects[i].y == blade_y)
            objects[i].type == TYPE_BOMB ? (game_over = 1) : (objects[i].active = 0, score += 10);
}

/* ------------------ RENDER ------------------ */
static void render(void) {
    if (!oldScreen_initialized) { memset(oldScreen, 0, sizeof(oldScreen)); oldScreen_initialized = 1; }
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            if (screen[y][x] != oldScreen[y][x]) {
                gotoxy(x, y); putchar(screen[y][x]); oldScreen[y][x] = screen[y][x];
            }
    gotoxy(0, HEIGHT);
    printf("Score:%-6d Level:%-2d  Q/ESC Quit                    ", score, current_level);
}

/* ------------------ MENU ------------------ */
static void showStartScreen(void) {
    const char* options[] = {
        "Level 1 : Classic", "Level 2 : Speed Rush",
        "Level 3 : Chaos Slice", "Quit"
    };

    int sel = 0;
    SHORT pu=0,pd=0,pe=0;

    system("cls");
    waitForKeyRelease();

    while (1) {
        gotoxy(0,0);
        printTitle();
        printf("\n           SELECT LEVEL\n\n");
        for (int i = 0; i < 4; i++)
            printf(i == sel ? "        > %s\n" : "          %s\n", options[i]);

        SHORT u = GetAsyncKeyState(VK_UP);
        SHORT d = GetAsyncKeyState(VK_DOWN);
        SHORT e = GetAsyncKeyState(VK_RETURN);

        if ((u & 0x8000) && !(pu & 0x8000)) sel = (sel + 3) % 4;
        if ((d & 0x8000) && !(pd & 0x8000)) sel = (sel + 1) % 4;
        if ((e & 0x8000) && !(pe & 0x8000)) {
            if (sel == 3) exit(0);
            configureLevel(sel + 1);
            waitForKeyRelease();
            return;
        }
        pu=u; pd=d; pe=e;
        Sleep(16);
    }
}

/* ------------------ MAIN ------------------ */
int main(void) {
    srand((unsigned)time(NULL));
    hideCursor();
    lockConsoleSize();

    while (1) {
        showStartScreen();
        resetGameState();

        while (!quit) {
            update();
            render();
            Sleep(FRAME_TIME_MS);
        }

        gotoxy(0, HEIGHT + 2);
        printf("Round Over! Score: %d\n", score);
        printf("Press ENTER to return to menu or ESC to quit.");
        waitForKeyRelease();

        SHORT prevE = 0, prevEsc = 0;
        while (1) {
            SHORT e = GetAsyncKeyState(VK_RETURN);
            SHORT esc = GetAsyncKeyState(VK_ESCAPE);
            if ((e & 0x8000) && !(prevE & 0x8000)) break;
            if ((esc & 0x8000) && !(prevEsc & 0x8000)) exit(0);
            prevE = e; prevEsc = esc;
            Sleep(16);
        }
    }
}
