#ifndef RLUT_HEADER
#define RLUT_HEADER
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// TODO: Input + event handling + forwarding
// TODO: More event/input callback functions

int rlutInit(int argc, const char *argv[]);
void rlutDisplayFunc(void(*func)(void));
void rlutReshapeFunc(void(*func)(int columns, int rows));
int rlutMainLoop(void);

// TODO: NCurses like cursor functions
// TODO: ImGUI wrapper functions

int* rlutCellularAutomataMap(int width, int height, int fillChance, int smoothIterations, int survive, int starve);
float* rlutPerlinNoiseMap(int width, int height, float z, float offsetX, float offsetY, float scale, float lacunarity, float gain, float octaves);
float rlutPerlinNoise(float x, float y, float z);

// TODO: RNG + Dice, A*, Poisson disc sampling

#ifdef __cplusplus
}
#endif
#endif // RLUT_HEADER
