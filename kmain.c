#include "io.h"
#include "gdt.h"
#include "interrupts.h"

int kmain() {
    init_gdt();
    configure_serial_port(SERIAL_COM1_BASE);
    interrupts_install_idt();
    log("This is my first log.");

    for(int x=0; x<8*25; x++)
        cout("helloworld");
    cout("helloworld");
    log("This is my 2irst log.");

    while(1)
        ;
    return 0x12345678;
}