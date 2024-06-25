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
// TODO: More event/input callback functions

int rlutInit(int argc, const char *argv[]);
void rlutDisplayFunc(void(*func)(void));
void rlutReshapeFunc(void(*func)(int columns, int rows));
void rlutKillLoop(void);
int rlutMainLoop(void);

void rlutClear(void);
void rlutMoveCursor(unsigned int x, unsigned int y);
void rlutScreenSize(unsigned int *width, unsigned int *height);
void rlutCursorPosition(unsigned int *x, unsigned int *y);
void rlutPrintChar(unsigned char ch);
void rlutPrintString(const char *fmt, ...);

void rlutSetSeed(uint64_t seed);
uint64_t rlutRandom(void);
float rlutRandomFloat(void);
int rlutRandomIntRange(int min, int max);
float rlutRandomFloatRange(float min, float max);

// TODO: NCurses like cursor functions
// TODO: ImGUI wrapper functions

uint8_t* rlutCellularAutomataMap(unsigned int width, unsigned int height, unsigned int fillChance, unsigned int smoothIterations, unsigned int survive, unsigned int starve);
uint8_t* rlutPerlinNoiseMap(unsigned int width, unsigned int height, float z, float offsetX, float offsetY, float scale, float lacunarity, float gain, float octaves);
float rlutPerlinNoise(float x, float y, float z);

// TODO: A*, Poisson disc sampling, FOV functions

#ifdef __cplusplus
}
#endif
#endif // RLUT_HEADER
