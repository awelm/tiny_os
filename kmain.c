#include "io.h"

int kmain() {
    //fb_write_cell(0, 'A', FB_GREEN, FB_DARK_GREY);
    char hello[5] = "hello";
    write(hello, 5);
    return 0x12345678;
}