#include "vcu.h"
#include <stdbool.h>

int main() {
    vcu_init();

    while (true) {
        vcu_main_loop();
    }

    return 0;
}
