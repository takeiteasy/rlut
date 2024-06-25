#include "rlut.h"
#include <stdlib.h>

static void display(void) {
    
}

int main(int argc, const char *argv[]) {
    if (!rlutInit(argc, argv))
        abort();
    rlutDisplayFunc(display);
    return rlutMainLoop();
}
