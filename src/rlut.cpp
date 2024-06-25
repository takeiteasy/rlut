#include "rlut.h"
#include <assert.h>
#include <ncurses.h>
#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

static struct {
    ImTui::TScreen* screen;
    void(*displayFunc)(void);
    void(*reshapeFunc)(int, int);
    bool running;
} rlut = {0};

int rlutInit(int argc, const char *argv[]) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    rlut.screen = ImTui_ImplNcurses_Init(true);
    ImTui_ImplText_Init();
    return 1;
}

void rlutDisplayFunc(void(*func)(void)) {
    rlut.displayFunc = func;
}

void rlutReshapeFunc(void(*func)(int columns, int rows)) {
    rlut.reshapeFunc = func;
}

int rlutMainLoop(void) {
    assert(rlut.displayFunc);
    rlut.running = true;
    while (rlut.running) {
        ImTui_ImplNcurses_NewFrame();
        ImTui_ImplText_NewFrame();
        ImGui::NewFrame();
        rlut.displayFunc();
        ImGui::Render();
        ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), rlut.screen);
        ImTui_ImplNcurses_DrawScreen();
    }
    ImTui_ImplText_Shutdown();
    ImTui_ImplNcurses_Shutdown();
    return 0;
}
