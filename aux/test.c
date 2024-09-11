#include "rlut.h"
#include <stdlib.h>
#include <stdio.h>

#define MAP_WIDTH 64
#define MAP_HEIGHT 64

typedef union {
    struct {
        uint8_t type;
        uint8_t solid;
        uint8_t visible;
        uint8_t color;
    };
    uint32_t value;
} TestTile;

static struct {
    TestTile map[MAP_WIDTH * MAP_HEIGHT];
} state;

static void display(void) {
    rlutClearScreen();
    rlutSetCursor(2, 5);
    rlutPrintString("\x1b[42;31mtest!\x1b[0m");
}

int main(int argc, const char *argv[]) {
    if (!rlutInit(argc, argv))
        abort();
    rlutDisplayFunc(display);
    return rlutMainLoop();
}
