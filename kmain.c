#include "io.h"
#include "gdt.h"

int kmain() {
    init_gdt();
    configure_serial_port(SERIAL_COM1_BASE);
    char logMsg[] = "This is my first log.";
    log(logMsg, strlen(logMsg));

    char world[]= "helloworld";
    for(int x=0; x<8*25; x++)
        cout(world);
    cout(world);
    char logMsgTwo[] = "This is my 2irst log.";
    log(logMsgTwo, 21);
    return 0x12345678;
}