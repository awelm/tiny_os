#include "io.h"

int kmain() {
    char world[10] = "helloworld";
    for(int x=0; x<8*25; x++)
        write(world, 10);
    write(world, 10);
    return 0x12345678;
}