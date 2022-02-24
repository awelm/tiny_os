#include "io.h"
#include "gdt.h"

int kmain() {
    init_gdt();
    configure_serial_port(SERIAL_COM1_BASE);
    log("This is my first log.");

    for(int x=0; x<8*25; x++)
        cout("helloworld");
    cout("helloworld");
    //char logMsgTwo[] = "This is my 2irst log.";
    log("This is my 2irst log.");
    return 0x12345678;
}