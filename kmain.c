#include "io.h"
#include "gdt.h"

int kmain() {
    init_gdt();
    configure_serial_port(SERIAL_COM1_BASE);
    char logMsg[21] = "This is my first log.";
    log(logMsg, 21);

    char world[10] = "helloworld";
    for(int x=0; x<8*25; x++)
        write(world, 10);
    write(world, 10);
    char logMsgTwo[21] = "This is my 2irst log.";
    log(logMsgTwo, 21);
    return 0x12345678;
}