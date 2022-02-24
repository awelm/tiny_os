#include "io.h"

int kmain() {
    char world[10] = "helloworld";
    for(int x=0; x<8*25; x++)
        write(world, 10);
    write(world, 10);
    configure_serial_port(SERIAL_COM1_BASE);
    for(int x=0; x<100; x++)
        write_to_serial_port(SERIAL_COM1_BASE, 'Z');
    return 0x12345678;
}