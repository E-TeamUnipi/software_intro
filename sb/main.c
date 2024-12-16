#include <main_db.h>
#include <nodes.h>

#include "sb.h"

int main()
{
    sb_init();

    while (true) {
        sb_main_loop();
    }

    return 0;
}

