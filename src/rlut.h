/* rlut.h -- https://www.github.com/takeiteasy/rlut
 
 RLUT is a utilities toolkit for making rogue-likes
 
 Copyright (C) 2024  George Watson
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>. */

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
    RLUT_HINT_DISABLE_UTF8, /* TODO */
    RLUT_HINT_DISABLE_RUNNING_COLOR,
    RLUT_HINT_INITIAL_SEED
};

#define RLUT_HINT_LAST RLUT_HINT_INITIAL_SEED

// TODO: Text Modes (bold, italics)
// TODO: Input + event handling + forwarding
// TODO: Try and generate wrapper for ImGui
// TODO: A*, Poisson disc sampling, FOV functions
// TODO: Alternate SDL GUI version (after TUI version is finished)
// TODO: UTF-8 support for PrintChar + PrintString
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
