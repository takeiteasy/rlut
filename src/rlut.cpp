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
#include <sstream>
#include <vector>

#if defined(RLUT_SDL2)
#if defined(_WIN32) || defined(_WIN64)
#define RLUT_WINDOWS
#include <windows.h>
#elif defined(__APPLE__) || defined(__MACH__)
#define RLUT_MAC
#include <AppKit/AppKit.h>
#else
#define RLUT_LINUX
#include <unistd.h>
#endif
#endif

inline std::uint8_t operator "" _u8(unsigned long long value) {
    return static_cast<std::uint8_t>(value);
}

#define CLAMP(V, MN, MX) (std::min(std::max((V), (MN)), (MN)))

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
    unsigned int savedCursorX, savedCursorY;
    uint8_t textMode;
    uint8_t backgroundColor;
    uint8_t foregroundColor;
    std::vector<std::vector<uint32_t>> screenBuffer;
} rlut = {0};

int rlutInit(int argc, const char *argv[]) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    rlut.screen = ImTui_ImplNcurses_Init(true);
    ImTui_ImplText_Init();
    rlutScreenSize(&rlut.screenW, &rlut.screenH);
    rlutClearScreen();
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
                rlut.screenBuffer.push_back(std::vector<uint32_t>(rlut.screenW, 0));
        } else
            rlut.screenBuffer.resize(rlut.screenH);
    }
    // Resize columns if width has changed
    if (rlut.screenW != lastW)
        for (int y = 0; y < rlut.screenH; y++) {
            if (lastW > rlut.screenW)
                rlut.screenBuffer[y].resize(rlut.screenW);
            else
                for (int i = 0; i < rlut.screenW - lastW; i++)
                    rlut.screenBuffer[y].push_back(0);
        }
    // Size changed, call reshape callback if it's set
    if ((rlut.screenW != lastW || rlut.screenH != lastH) && rlut.reshapeFunc)
        rlut.reshapeFunc(rlut.screenW, rlut.screenH);
}

void rlutKillLoop(void) {
    rlut.running = false;
}

union Cell {
    struct {
        uint8_t ch;
        uint8_t fg;
        uint8_t bg;
        uint8_t mode;
    };
    uint32_t value;
};

static void ResetTextStyle(void) {
    rlut.textMode = 0;
    rlut.backgroundColor = 0;
    rlut.foregroundColor = 7;
}

int rlutMainLoop(void) {
    assert(rlut.displayFunc);
    rlut.running = true;
    while (rlut.running) {
        ImTui_ImplNcurses_NewFrame();
        ImTui_ImplText_NewFrame();
        ImGui::NewFrame();
        ResizeScreenBuffer();
        ResetTextStyle();
        
        if (rlut.preframeFunc)
            rlut.preframeFunc();
        
        rlut.displayFunc();
        
        ImGui::Begin("test");
        ImGui::Text("Hello, world!");
        ImGui::End();
        
        ImGui::Render();
        ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), rlut.screen);
        ImTui_ImplNcurses_DrawScreen();
        
        init_pair(1, COLOR_BLACK, COLOR_RED);
        attron(COLOR_PAIR(1));
        mvprintw(0, 0, "test\n");

        if (rlut.postframeFunc)
            rlut.postframeFunc();
    }
    
    if (rlut.atExitFunc)
        rlut.atExitFunc();
    ImTui_ImplText_Shutdown();
    ImTui_ImplNcurses_Shutdown();
    return 0;
}

void rlutClearScreen(void) {
    rlut.screenBuffer.clear();
    rlut.screenBuffer.reserve(rlut.screenH);
    for (int y = 0; y < rlut.screenH; y++)
        rlut.screenBuffer.push_back(std::vector<uint32_t>(rlut.screenW, 0));
    rlut.cursorX = rlut.cursorY = 0;
}

void rlutMoveCursor(int x, int y) {
    if (y != 0) {
        int dy = rlut.cursorY + y;
        rlut.cursorY = CLAMP(dy, 0, rlut.screenH - 1);
    }
    if (x != 0) {
        int dx = rlut.cursorX + x;
        if (dx < 0) {
            // Underflow, wrap backwards to previous line unless on first line
            if (rlut.cursorY == 0)
                rlut.cursorX = 0;
            else {
                rlut.cursorY--;
                rlut.cursorX = rlut.screenW + dx;
            }
        }
        else if (dx >= rlut.screenW) {
            // Overflow, wrap forwards to next line unless on last line
            if (rlut.cursorY == rlut.screenH - 1)
                rlut.cursorX = rlut.screenW - 1;
            else {
                rlut.cursorY++;
                rlut.cursorX = dx - rlut.screenW;
            }
        } else
            rlut.cursorX = dx; // No wrapping needed
    }
}

// NOTE: Coordinates set with SetCursor won't be wrapped but will be clamped to
//       the screen bounds.
void rlutSetCursor(unsigned int x, unsigned int y) {
    rlut.cursorX = std::min(rlut.screenW-1, x);
    rlut.cursorY = std::min(rlut.screenH-1, y);
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

void rlutPrintChar(uint8_t ch, uint8_t mode, uint8_t fg, uint8_t bg) {
    assert(rlut.cursorX >= 0 && rlut.cursorY >= 0 && rlut.cursorX < rlut.screenW && rlut.cursorY < rlut.screenH);
    Cell cell = {
        .ch = ch,
        .fg = CLAMP(fg, 0_u8, 9_u8),
        .bg   = CLAMP(bg, 0_u8, 9_u8),
        .mode = CLAMP(mode, 0_u8, 9_u8)
    };
    rlut.screenBuffer[rlut.cursorY][rlut.cursorX] = cell.value;
    rlutMoveCursor(+1, 0);
}

// Attempt to read the next token in the ANSI escape sequence
static bool ParseNextANSIEscapeToken(char *p, bool *isInteger, uint8_t *value, size_t *length) {
    // Check if unexpected eol
    if (!p || !*p)
        return false;
    bool isInt = true;
    int i = 0;
    char buf[3] = {0};
    switch (*p) {
        // First character is a number, attempt to read up to 3 digits (0-255)
        case '0' ... '9': {
            for (; i < 3; i++) {
                char c = *(p + i);
                if (c < '0' || c > '9') {
                    if (c == ';' ||
                        (c >= 'a' && c <= 'z') ||
                        (c >= 'A' && c <= 'Z'))
                        break;
                    else
                        return false; // error, invalid integer
                }
                // Valid integer, store in buffer
                buf[i] = c;
            }
            break;
        }
        // Not a number, could be the mode
        case 'a' ... 'z':
        case 'A' ... 'Z':
            buf[0] = *p;
            i = 1;
            isInt = false;
            break;
        // Not a-z, A-Z or 0-9 (invalid)
        default:
            return false; // error, invalid character
    }
    // Return token length, if it's an integer and the token value
    if (length)
        *length = i;
    if (isInteger)
        *isInteger = isInt;
    if (value) {
        if (isInt) {
            int v = std::stoi(std::string(buf));
            if (v < 0 || v > UINT8_MAX)
                return false;
            *value = static_cast<uint8_t>(v);
        } else
            *value = *(char*)&buf;
    }
    return true;
}

static void ClearLineToEnd(void) {
    for (int x = rlut.cursorX; x < rlut.screenW - 1; x++)
        rlut.screenBuffer[rlut.cursorY][x] = 0;
}

static void ClearLineToCursor(void) {
    for (int x = 0; x < rlut.cursorX; x++)
        rlut.screenBuffer[rlut.cursorY][x] = 0;
}

static void ClearLine(int y) {
    std::fill(rlut.screenBuffer[y].begin(), rlut.screenBuffer[y].end(), 0);
}

// Attempt to parse an ANSI escape sequence
static char* ParseANSIEscape(char *_p) {
    // Check if the first character is not null and = '['
    if (!_p || !*_p || *_p != '[')
        return _p;
    char *p = (char*)++_p;
    int n = 0;
    // Store values in this buffer, maximum of 4 tokens, the mode and up to 3 integer values
    uint8_t tmp[4] = {0};
    // Keeps track if the current token is an integer value or the mode, if it's the mode
    // start to evaluate with the integer values (if any)
    bool isInteger;
    // Keep track of how far we have read into the string
    size_t totalLength = 1, tokenLength = 0;
    while (n < 4 && ParseNextANSIEscapeToken(p, &isInteger, &tmp[n], &tokenLength)) {
        // If the token is an integer, check for a semi-colon delimeter and skip to
        // the next token
        if (isInteger) {
            p += tokenLength;
            totalLength += tokenLength;
            char next = *p;
            if ((next < 'a' && next > 'z') &&
                (next < 'A' && next > 'Z') &&
                next != ';') // error, expecting semi-colon delimeter
                return _p;
            if (next == ';')
                p++;
            n++;
            continue; // next token
        }
        
        // The token isn't an integer, check if it's a valid mode
        switch (tmp[n]) {
            case 'A': // Move cursor up
                rlutMoveCursor(0, -1 * (n ? tmp[0] : 1));
                break;
            case 'B': // Move cursor down
                rlutMoveCursor(0, +1 * (n ? tmp[0] : 1));
                break;
            case 'C': // Move cursor forward
                rlutMoveCursor(+1 * (n ? tmp[0] : 1), 0);
                break;
            case 'D': // Move cursor back
                rlutMoveCursor(-1 * (n ? tmp[0] : 1), 0);
                break;
            case 'E': { // Cursor next line
                int dy = rlut.cursorY + 1 * (n ? tmp[0] : 1);
                int mh = rlut.screenH - 1;
                rlut.cursorY = std::max(dy, mh);
                rlut.cursorX = 0;
                break;
            }
            case 'F': { // Cursor previous line
                int dy = rlut.cursorY + -1 * (n ? tmp[0] : 1);
                rlut.cursorY = std::max(dy, 0);
                rlut.cursorX = 0;
                break;
            }
            case 'G': // Cursor horizontal absolute
                rlutSetCursor(n ? tmp[0] : 0, rlut.cursorY);
                break;
            case 'H': // Cursor position
                switch (n) {
                    case 0:
                        rlutSetCursor(0, 0);
                        break;
                    case 1:
                        rlutSetCursor(tmp[0], rlut.cursorY);
                        break;
                    default:
                    case 2:
                        rlutSetCursor(tmp[0], tmp[1]);
                        break;
                }
                break;
            case 'J': { // Erase in Display
                int mode = 0;
                switch (n) {
                    default:
                    case 1:
                        mode = tmp[0];
                    case 0:
                        switch (mode) {
                            case 0: // Clear cursor to end of screen
                                ClearLineToEnd();
                                for (int y = rlut.cursorY; y < rlut.screenH - 1; y++)
                                    ClearLine(y);
                                break;
                            case 1: // Clear from cursor to beginning of screen
                                ClearLineToCursor();
                                for (int y = 0; y < rlut.cursorY; y++)
                                    ClearLine(y);
                                    break;
                            case 3: // Same as 2, but deletes scrollback buffer (we have no scrollback buffer)
                            case 2: // Clear entire screen and move cursor to 0,0
                                rlutClearScreen();
                                rlutSetCursor(0, 0);
                                break;
                            default: // error, invalid mode
                                return _p;
                        }
                        break;
                }
                break;
            }
            case 'K': { // Erase in Line
                int mode = 0;
                switch (n) {
                    default:
                    case 1:
                        mode = tmp[0];
                    case 0:
                        switch (mode) {
                            case 0: // clear from cursor to eol
                                ClearLineToEnd();
                                break;
                            case 1: // clear from cursor to start of line
                                ClearLineToCursor();
                                break;
                            case 2: // clear the entire line
                                ClearLine(rlut.cursorY);
                                break;
                            default: // error, invalid mode
                                return _p;
                        }
                        break;
                }
                break;
            }
            case 'm': // Select Graphic Rendition
                if (!n)
                    ResetTextStyle();
                else
                    for (int i = 0; i < std::min(n, 4); i++)
                        switch (tmp[i]) {
                            case 0 ... 9: // Text modes
                                if (!(rlut.textMode = tmp[i]))
                                    ResetTextStyle();
                                break;
                            case 30 ... 39: // Foreground colors
                                rlut.foregroundColor = tmp[i] - 30;
                                break;
                            case 40 ... 49: // Background colors
                                rlut.backgroundColor = tmp[i] - 40;
                                break;
                            default: // Unsupported, skip
                                return _p;
                        }
                break;
            case 'n': // Device Status Report
            case 's': // Save Current Cursor Position
                rlut.savedCursorX = rlut.cursorX;
                rlut.savedCursorY = rlut.cursorY;
                break;
            case 'u': // Restore Saved Cursor Position
                rlut.cursorX = rlut.savedCursorX;
                rlut.cursorY = rlut.savedCursorY;
                break;
            default: // error, invalid mode
                return _p;
        }
        // Valid mode, break out of loop and return adjusted pointer
        break;
    }
    return _p + totalLength + tokenLength;
}

void rlutPrintString(const char *fmt, ...) {
    // Format a string into a string buffer
    va_list args;
    va_start(args, fmt);
    std::stringstream ss;
    int length = vsnprintf(NULL, 0, fmt, args);
    std::vector<char> buffer(length + 1);
    va_end(args);
    va_start(args, fmt);
    vsnprintf(buffer.data(), buffer.size(), fmt, args);
    va_end(args);
    ss << buffer.data();
    std::string str = ss.str();
    // Loop through string and parse any ANSI escapes and control codes
    const char *_p = str.c_str();
    char *p = (char*)_p;
    for (; *p; p++) {
        switch (*p) {
            case '\a': // Bell
                rlutBeep();
                break;
            case '\b': // Backspace
                rlutMoveCursor(-1, 0);
                break;
            case '\e': // Escape sequence
                p = ParseANSIEscape(p + 1);
                break;
            case '\n': // Line Feed
                rlut.cursorX = 0;
            case '\f':
            case '\v': // Form feed + Vertical tab
                if (rlut.cursorY != rlut.screenH - 1)
                    rlut.cursorY++;
                break;
            case '\r': // Carriage Return
                rlut.cursorX = 0;
                break;
            case '\t': // Tab
                if (rlut.cursorX == rlut.screenW - 1) {
                    if (rlut.cursorY != rlut.screenH - 1) {
                        rlut.cursorY++;
                        rlut.cursorX = 0;
                    }
                } else
                    rlut.cursorX = std::min((rlut.cursorX + 7) & ~7, rlut.screenW - 1);
                break;
            // Not an escape or control code, probably a character, print it
            default:
                rlutPrintChar(*p, rlut.textMode, rlut.foregroundColor, rlut.backgroundColor);
                break;
        }
    }
}

#if defined(RLUT_SDL2)
void rlutBeep(void) {
#if defined(RLUT_WINDOWS)
    SystemSoundsBeep(TONE_750, 250);
#elif defined(RLUT_MAC)
    NSBeep();
#else
    int fd = open("/dev/console", O_WRONLY);
    if (fd == -1)
        return;
    unsigned int value = (750 << 16) | 250;
    int ret = ioctl(fd, KIOCSOUND, &value);
    close(fd);
#endif
}
#else
void rlutBeep(void) {
    beep();
}
#endif

void rlutSetSeed(uint64_t seed) {
    rlut.seed = seed ? seed : time(NULL);
}

uint64_t rlutRandom(void) {
    assert(rlut.running && rlut.seed);
    rlut.seed = rlut.seed * 6364136223846793005ULL + 1;
    return rlut.seed;
}

float rlutRandomFloat(void) {
    return static_cast<float>(rlutRandom()) / static_cast<float>(std::numeric_limits<uint64_t>::max());
}

int rlutRandomIntRange(int min, int max) {
    if (min > max)
        std::swap(min, max);
    return static_cast<int>(rlutRandomFloat() * (max - min + 1) + min);
}

float rlutRandomFloatRange(float min, float max) {
    if (min > max)
        std::swap(min, max);
    return rlutRandomFloat() * (max - min) + min;
}

uint8_t* rlutCellularAutomataMap(unsigned int width, unsigned int height, unsigned int fillChance, unsigned int smoothIterations, unsigned int survive, unsigned int starve) {
    assert(width && height);
    size_t sz = width * height * sizeof(int);
    uint8_t *result = (uint8_t*)RLUT_MALLOC(sz);
    memset(result, 0, sz);
    // Randomly fill the grid
    fillChance = CLAMP(fillChance, 1u, 99u);
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            result[y * width + x] = rlutRandom() % 100 + 1 < fillChance;
    // Run cellular-automata on grid n times
    for (int i = 0; i < std::max(smoothIterations, 1u); i++)
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
