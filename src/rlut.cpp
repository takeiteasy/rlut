/* rlut.cpp -- https://www.github.com/takeiteasy/rlut
 
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
#include <assert.h>
#include <ncurses.h>
#include <time.h>
#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"
#include <limits>
#include <algorithm>
#include <string>
#include <vector>

static struct {
    ImTui::TScreen* screen;
    void(*displayFunc)(void);
    void(*preframeFunc)(void);
    void(*postframeFunc)(void);
    void(*reshapeFunc)(int, int);
    void(*atExitFunc)(void);
    bool running;
    uint64_t seed;
    unsigned int screenW, screenH;
    unsigned int cursorX, cursorY;
    unsigned int cameraX, cameraY;
    std::vector<std::string> screenBuffer;
} rlut = {0};

int rlutInit(int argc, const char *argv[]) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    rlut.screen = ImTui_ImplNcurses_Init(true);
    ImTui_ImplText_Init();
    rlutScreenSize(&rlut.screenW, &rlut.screenH);
    rlut.screenBuffer.reserve(rlut.screenH);
    for (int y = 0; y < rlut.screenH; y++)
        rlut.screenBuffer.push_back(std::string(rlut.screenW, ' '));
    rlut.cursorX = rlut.cursorY = 0;
    rlut.cameraX = rlut.cameraY = 0;
    rlutSetSeed(0);
    return 1;
}

void rlutDisplayFunc(void(*func)(void)) {
    rlut.displayFunc = func;
}

void rlutPreframeFunc(void(*func)(void)) {
    rlut.preframeFunc = func;
}

void rlutPostframeFunc(void(*func)(void)) {
    rlut.postframeFunc = func;
}

void rlutReshapeFunc(void(*func)(int columns, int rows)) {
    rlut.reshapeFunc = func;
}

void rlutAtExit(void(*func)(void)) {
    rlut.atExitFunc = func;
}

static void ResizeScreenBuffer(void) {
    int lastW = rlut.screenW, lastH = rlut.screenH;
    rlutScreenSize(&rlut.screenW, &rlut.screenH);
    // Resize rows if height has changed
    if (rlut.screenH != lastH) {
        if (rlut.screenH > lastH) {
            for (int i = 0; i < rlut.screenH - lastH; i++)
                rlut.screenBuffer.push_back(std::string(rlut.screenW, ' '));
        } else
            rlut.screenBuffer.resize(rlut.screenH);
    }
    // Resize columns if width has changed
    if (rlut.screenW != lastW)
        for (int y = 0; y < rlut.screenH; y++) {
            if (lastW > rlut.screenW)
                rlut.screenBuffer[y].resize(rlut.screenW);
            else
                rlut.screenBuffer[y] += std::string(rlut.screenW - lastW, ' ');
        }
    // Size changed, call reshape callback if it's set
    if ((rlut.screenW != lastW || rlut.screenH != lastH) && rlut.reshapeFunc)
        rlut.reshapeFunc(rlut.screenW, rlut.screenH);
}

void rlutKillLoop(void) {
    rlut.running = false;
}

int rlutMainLoop(void) {
    assert(rlut.displayFunc);
    rlut.running = true;
    while (rlut.running) {
        ImTui_ImplNcurses_NewFrame();
        ImTui_ImplText_NewFrame();
        ImGui::NewFrame();
        
        if (rlut.preframeFunc)
            rlut.preframeFunc();
        
        ResizeScreenBuffer();
        rlut.displayFunc();
        
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x000000FF);
        if (ImGui::Begin("screen", NULL,
                        ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoDecoration |
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    ImVec2 mpos = ImGui::GetMousePos();
                    rlut.cameraX = mpos.x - (rlut.screenW / 2);
                    rlut.cameraY = mpos.y - (rlut.screenH / 2);
                }
                if (ImGui::IsKeyDown('w'))
                    rlut.cameraY -= 5;
                if (ImGui::IsKeyDown('a'))
                    rlut.cameraX -= 5;
                if (ImGui::IsKeyDown('s'))
                    rlut.cameraY += 5;
                if (ImGui::IsKeyDown('d'))
                    rlut.cameraX += 5;
            }
            
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 1.f, 0.f, 1.f));
            ImGui::Text("this is the first line");
            ImGui::PopStyleColor();
            ImGui::SameLine(0, 0);
            ImGui::Text("this is thhe second line");
        }
        ImGui::PopStyleColor();
        ImGui::End();
        
        ImGui::Render();
        ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), rlut.screen);
        ImTui_ImplNcurses_DrawScreen();
        
        if (rlut.postframeFunc)
            rlut.postframeFunc();
    }
    
    if (rlut.atExitFunc)
        rlut.atExitFunc();
    ImTui_ImplText_Shutdown();
    ImTui_ImplNcurses_Shutdown();
    return 0;
}

void rlutClear(void) {
    
}

void rlutMoveCursor(unsigned int x, unsigned int y) {
    
}

void rlutScreenSize(unsigned int *width, unsigned int *height) {
    unsigned int col, row;
    getmaxyx(stdscr, row, col);
    if (width)
        *width = col;
    if (height)
        *height = row;
}

void rlutCursorPosition(unsigned int *x, unsigned int *y) {
    if (x)
        *x = rlut.cursorX;
    if (y)
        *y = rlut.cursorY;
}

void rlutPrintChar(unsigned char ch) {
    
}

void rlutPrintString(const char *fmt, ...) {
    
}

void rlutSetSeed(uint64_t seed) {
    rlut.seed = seed ? seed : time(NULL);
}

uint64_t rlutRandom(void) {
    assert(rlut.running && rlut.seed);
    rlut.seed = rlut.seed * 6364136223846793005ULL + 1;
    return rlut.seed;
}

float rlutRandomFloat(void) {
    static const uint64_t max_uint64 = std::numeric_limits<uint64_t>::max();
    uint64_t random_uint64 = rlutRandom();
    return static_cast<float>(random_uint64) / static_cast<float>(max_uint64);
}

int rlutRandomIntRange(int min, int max) {
    if (min > max)
        std::swap(min, max);
    return static_cast<int>(rlutRandomFloat() * (max - min + 1)) + min;
}

float rlutRandomFloatRange(float min, float max) {
    if (min > max)
        std::swap(min, max);
    return rlutRandomFloat() * (max - min) + min;
}

template <typename T> T clamp(const T& value, const T& min, const T& max) {
    return std::max(min, std::min(value, max));
}

uint8_t* rlutCellularAutomataMap(unsigned int width, unsigned int height, unsigned int fillChance, unsigned int smoothIterations, unsigned int survive, unsigned int starve) {
    assert(width && height);
    size_t sz = width * height * sizeof(int);
    uint8_t *result = (uint8_t*)RLUT_MALLOC(sz);
    memset(result, 0, sz);
    // Randomly fill the grid
    fillChance = clamp(static_cast<int>(fillChance), 1, 99);
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            result[y * width + x] = rlutRandom() % 100 + 1 < fillChance;
    // Run cellular-automata on grid n times
    for (int i = 0; i < smoothIterations; i++)
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                // Count the cell's living neighbours
                int neighbours = 0;
                for (int nx = x - 1; nx <= x + 1; nx++)
                    for (int ny = y - 1; ny <= y + 1; ny++)
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            if ((nx != x || ny != y) && result[ny * width + nx] > 0)
                                neighbours++;
                        } else
                            neighbours++;
                // Update cell based on neighbour and surive/starve values
                if (neighbours > survive)
                    result[y * width + x] = 1;
                else if (neighbours < starve)
                    result[y * width + x] = 0;
            }
    return result;
}

static float remap(float value, float from1, float to1, float from2, float to2) {
    return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
}

uint8_t* rlutPerlinNoiseMap(unsigned int width, unsigned int height, float z, float offsetX, float offsetY, float scale, float lacunarity, float gain, float octaves) {
    float min = FLT_MAX, max = FLT_MIN;
    float *grid = (float*)RLUT_MALLOC(width * height * sizeof(float));
    // Loop through grid and apply noise transformation to each cell
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y) {
            float freq = 2.f,
            amp  = 1.f,
            tot  = 0.f,
            sum  = 0.f;
            for (int i = 0; i < octaves; ++i) {
                sum  += rlutPerlinNoise(((offsetX + x) / scale) * freq, ((offsetY + y) / scale) * freq, z) * amp;
                tot  += amp;
                freq *= lacunarity;
                amp  *= gain;
            }
            // Keep track of min + max values for remapping the range later
            grid[y * width + x] = sum = (sum / tot);
            if (sum < min)
                min = sum;
            if (sum > max)
                max = sum;
        }
    // Convert float values to 0-255 range
    uint8_t *result = (uint8_t*)RLUT_MALLOC(width * height * sizeof(uint8_t));
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            float height = 255.f - (255.f * remap(grid[y * width + x], min, max, 0, 1.f));
            result[y * width + x] = (unsigned char)height;
        }
    // Free float grid, return uint8 grid
    RLUT_FREE(grid);
    return result;
}

static const float grad3[][3] = {
    { 1, 1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 },
    { 1, 0, 1 }, { -1, 0, 1 }, { 1, 0, -1 }, { -1, 0, -1 },
    { 0, 1, 1 }, { 0, -1, 1 }, { 0, 1, -1 }, { 0, -1, -1 }
};

static const unsigned int perm[] = {
    182, 232, 51, 15, 55, 119, 7, 107, 230, 227, 6, 34, 216, 61, 183, 36,
    40, 134, 74, 45, 157, 78, 81, 114, 145, 9, 209, 189, 147, 58, 126, 0,
    240, 169, 228, 235, 67, 198, 72, 64, 88, 98, 129, 194, 99, 71, 30, 127,
    18, 150, 155, 179, 132, 62, 116, 200, 251, 178, 32, 140, 130, 139, 250, 26,
    151, 203, 106, 123, 53, 255, 75, 254, 86, 234, 223, 19, 199, 244, 241, 1,
    172, 70, 24, 97, 196, 10, 90, 246, 252, 68, 84, 161, 236, 205, 80, 91,
    233, 225, 164, 217, 239, 220, 20, 46, 204, 35, 31, 175, 154, 17, 133, 117,
    73, 224, 125, 65, 77, 173, 3, 2, 242, 221, 120, 218, 56, 190, 166, 11,
    138, 208, 231, 50, 135, 109, 213, 187, 152, 201, 47, 168, 185, 186, 167, 165,
    102, 153, 156, 49, 202, 69, 195, 92, 21, 229, 63, 104, 197, 136, 148, 94,
    171, 93, 59, 149, 23, 144, 160, 57, 76, 141, 96, 158, 163, 219, 237, 113,
    206, 181, 112, 111, 191, 137, 207, 215, 13, 83, 238, 249, 100, 131, 118, 243,
    162, 248, 43, 66, 226, 27, 211, 95, 214, 105, 108, 101, 170, 128, 210, 87,
    38, 44, 174, 188, 176, 39, 14, 143, 159, 16, 124, 222, 33, 247, 37, 245,
    8, 4, 22, 82, 110, 180, 184, 12, 25, 5, 193, 41, 85, 177, 192, 253,
    79, 29, 115, 103, 142, 146, 52, 48, 89, 54, 121, 212, 122, 60, 28, 42,
    
    182, 232, 51, 15, 55, 119, 7, 107, 230, 227, 6, 34, 216, 61, 183, 36,
    40, 134, 74, 45, 157, 78, 81, 114, 145, 9, 209, 189, 147, 58, 126, 0,
    240, 169, 228, 235, 67, 198, 72, 64, 88, 98, 129, 194, 99, 71, 30, 127,
    18, 150, 155, 179, 132, 62, 116, 200, 251, 178, 32, 140, 130, 139, 250, 26,
    151, 203, 106, 123, 53, 255, 75, 254, 86, 234, 223, 19, 199, 244, 241, 1,
    172, 70, 24, 97, 196, 10, 90, 246, 252, 68, 84, 161, 236, 205, 80, 91,
    233, 225, 164, 217, 239, 220, 20, 46, 204, 35, 31, 175, 154, 17, 133, 117,
    73, 224, 125, 65, 77, 173, 3, 2, 242, 221, 120, 218, 56, 190, 166, 11,
    138, 208, 231, 50, 135, 109, 213, 187, 152, 201, 47, 168, 185, 186, 167, 165,
    102, 153, 156, 49, 202, 69, 195, 92, 21, 229, 63, 104, 197, 136, 148, 94,
    171, 93, 59, 149, 23, 144, 160, 57, 76, 141, 96, 158, 163, 219, 237, 113,
    206, 181, 112, 111, 191, 137, 207, 215, 13, 83, 238, 249, 100, 131, 118, 243,
    162, 248, 43, 66, 226, 27, 211, 95, 214, 105, 108, 101, 170, 128, 210, 87,
    38, 44, 174, 188, 176, 39, 14, 143, 159, 16, 124, 222, 33, 247, 37, 245,
    8, 4, 22, 82, 110, 180, 184, 12, 25, 5, 193, 41, 85, 177, 192, 253,
    79, 29, 115, 103, 142, 146, 52, 48, 89, 54, 121, 212, 122, 60, 28, 42
};

static float dot3(const float a[], float x, float y, float z) {
    return a[0]*x + a[1]*y + a[2]*z;
}

static float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

static float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

#define FASTFLOOR(x) (((x) >= 0) ? (int)(x) : (int)(x)-1)

float rlutPerlinNoise(float x, float y, float z) {
    /* Find grid points */
    int gx = FASTFLOOR(x);
    int gy = FASTFLOOR(y);
    int gz = FASTFLOOR(z);
    
    /* Relative coords within grid cell */
    float rx = x - gx;
    float ry = y - gy;
    float rz = z - gz;
    
    /* Wrap cell coords */
    gx = gx & 255;
    gy = gy & 255;
    gz = gz & 255;
    
    /* Calculate gradient indices */
    unsigned int gi[8];
    for (int i = 0; i < 8; i++)
        gi[i] = perm[gx+((i>>2)&1)+perm[gy+((i>>1)&1)+perm[gz+(i&1)]]] % 12;
    
    /* Noise contribution from each corner */
    float n[8];
    for (int i = 0; i < 8; i++)
        n[i] = dot3(grad3[gi[i]], rx - ((i>>2)&1), ry - ((i>>1)&1), rz - (i&1));
    
    /* Fade curves */
    float u = fade(rx);
    float v = fade(ry);
    float w = fade(rz);
    
    /* Interpolate */
    float nx[4];
    for (int i = 0; i < 4; i++)
        nx[i] = lerp(n[i], n[4+i], u);
    
    float nxy[2];
    for (int i = 0; i < 2; i++)
        nxy[i] = lerp(nx[i], nx[2+i], v);
    
    return lerp(nxy[0], nxy[1], w);
}
