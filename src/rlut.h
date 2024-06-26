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

// TODO: Input + event handling + forwarding
// TODO: ImGUI wrapper functions
// TODO: A*, Poisson disc sampling, FOV functions

// Windows + context functions
int rlutInit(int argc, const char *argv[]);
void rlutDisplayFunc(void(*func)(void));
void rlutPreframeFunc(void(*func)(void));
void rlutPostframeFunc(void(*func)(void));
void rlutReshapeFunc(void(*func)(int columns, int rows));
void rlutAtExit(void(*func)(void));
void rlutKillLoop(void);
int rlutMainLoop(void);
// Cursor + screen state functions
void rlutClear(void);
void rlutMoveCursor(int x, int y);
void rlutSetCursor(unsigned int x, unsigned int y);
void rlutScreenSize(unsigned int *width, unsigned int *height);
void rlutCursorPosition(unsigned int *x, unsigned int *y);
void rlutPrintChar(uint8_t ch, uint8_t fgR, uint8_t fgG, uint8_t fgB);
void rlutPrintString(const char *fmt, ...);
// RNG + seed functions
void rlutSetSeed(uint64_t seed);
uint64_t rlutRandom(void);
float rlutRandomFloat(void);
int rlutRandomIntRange(int min, int max);
float rlutRandomFloatRange(float min, float max);
// Map generation + noise functions
uint8_t* rlutCellularAutomataMap(unsigned int width, unsigned int height, unsigned int fillChance, unsigned int smoothIterations, unsigned int survive, unsigned int starve);
uint8_t* rlutPerlinNoiseMap(unsigned int width, unsigned int height, float z, float offsetX, float offsetY, float scale, float lacunarity, float gain, float octaves);
float rlutPerlinNoise(float x, float y, float z);

#ifdef __cplusplus
}
#endif
#endif // RLUT_HEADER
