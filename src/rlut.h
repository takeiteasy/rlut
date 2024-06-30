/* rlut.h -- https://www.github.com/takeiteasy/rlut
 
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

#ifndef RLUT_HEADER
#define RLUT_HEADER
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdarg.h>

#ifndef RLUT_MALLOC
#define RLUT_MALLOC malloc
#endif
#ifndef RLUT_FREE
#define RLUT_FREE free
#endif

enum {
    RLUT_TEXT_DEFAULT = -1,
    RLUT_TEXT_RESET = 0,
    RLUT_TEXT_BOLD,
    RLUT_TEXT_DIM,
    RLUT_TEXT_ITALIC,
    RLUT_TEXT_UNDERLINE,
    RLUT_TEXT_BLINKING,
    RLUT_TEXT_INVERSE,
    RLUT_TEXT_HIDDEN,
    RLUT_TEXT_STRIKETHRU
};

enum {
    RLUT_COLOR_DEFAULT = 1,
    RLUT_COLOR_BLACK = 0,
    RLUT_COLOR_RED = 9,
    RLUT_COLOR_GREEN = 10,
    RLUT_COLOR_YELLOW = 11,
    RLUT_COLOR_BLUE = 12,
    RLUT_COLOR_MAGENTA = 13,
    RLUT_COLOR_CYAN = 14,
    RLUT_COLOR_WHITE = 15
};

enum {
    RLUT_HINT_DEFAULT_FOREGROUND_COLOR,
    RLUT_HINT_DEFAULT_BACKGROUND_COLOR,
    RLUT_HINT_WINDOW_WIDTH, /* GUI version only */
    RLUT_HINT_WINDOW_HEIGHT, /* GUI version only */
    RLUT_HINT_DISABLE_TEXT_WRAP,
    RLUT_HINT_DISABLE_TEXT_AUTO_ADVANCE,
    RLUT_HINT_ENABLE_Y_WRAP,
    RLUT_HINT_DISABLE_UTF8,
    RLUT_HINT_DISABLE_RUNNING_STATE,
    RLUT_HINT_INITIAL_SEED
};

#define RLUT_HINT_LAST RLUT_HINT_INITIAL_SEED

// TODO: Rewrite screen buffer resizing function
// TODO: Text Modes (bold, italics)
// TODO: Input + event handling + forwarding
// TODO: Try and generate wrapper for ImGui
// TODO: A*, Poisson disc sampling, FOV functions
// TODO: Alternate SDL GUI version (after TUI version is finished)
// TODO: UTF-8 support to PrintChar + PrintString
// TODO: Panel API
//  - Panels are boxes that you can write text to like the main buffer
//  - API will handle rendering + text-wrapping
//  - Push/Pop immediate mode style
// TODO: Build without ImGui/ImTui option
// TODO: Simple event emitter

// Windows + context functions
int rlutInit(int argc, const char *argv[]);
void rlutSetHint(unsigned int key, int val);
void rlutDisplayFunc(void(*func)(void));
void rlutPreframeFunc(void(*func)(void));
void rlutPostframeFunc(void(*func)(void));
void rlutReshapeFunc(void(*func)(int columns, int rows));
void rlutAtExit(void(*func)(void));
void rlutKillLoop(void);
int rlutMainLoop(void);
void rlutBeep(void);

// Cursor + screen state functions
void rlutClearScreen(void);
void rlutMoveCursor(int x, int y);
void rlutSetCursor(unsigned int x, unsigned int y);
void rlutScreenSize(unsigned int *width, unsigned int *height);
void rlutCursorPosition(unsigned int *x, unsigned int *y);
void rlutPrintChar(uint32_t ch, int8_t mode, uint8_t foregroundColor, uint8_t backgroundColor);
void rlutPrintString(const char *fmt, ...);

// RNG + seed functions
void rlutSetSeed(uint64_t seed);
uint64_t rlutRandom(void);
float rlutRandomFloat(void);
int rlutRandomIntRange(int min, int max);
float rlutRandomFloatRange(float min, float max);

// Map + noise functions
uint8_t* rlutCellularAutomataMap(unsigned int width, unsigned int height, unsigned int fillChance, unsigned int smoothIterations, unsigned int survive, unsigned int starve);
uint8_t* rlutPerlinNoiseMap(unsigned int width, unsigned int height, float z, float offsetX, float offsetY, float scale, float lacunarity, float gain, float octaves);
float rlutPerlinNoise(float x, float y, float z);

#ifdef __cplusplus
}
#endif
#endif // RLUT_HEADER
