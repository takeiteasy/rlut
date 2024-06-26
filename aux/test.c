/* test.c -- https://www.github.com/takeiteasy/rlut
 
 The MIT License (MIT)

 Copyright (c) 2024 George Watson

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge,
 publish, distribute, sublicense, and/or sell copies of the Software,
 and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

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
