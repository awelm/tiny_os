#include "io.h"
#include "gdt.h"
#include "interrupts.h"
#include "multiboot.h"

typedef void (*func_ptr)(void);

void start_user_program(multiboot_info_t *mbinfo) {
    if(mbinfo->mods_count == 1) {
        module_t* modules = (module_t*) mbinfo->mods_addr;
        unsigned int address_of_module = modules->mod_start; 

	    func_ptr start_program = (func_ptr) address_of_module;
    	start_program();
        /* we'll never get here, unless the module code returns */
    } else {
        log("Multiple user programs included. Unsure which program to jump to");
    }
}

int kmain(multiboot_info_t *mbinfo) {
    init_gdt();
    configure_serial_port(SERIAL_COM1_BASE);
    interrupts_install_idt();
    log("This is my first log.");

    for(int x=0; x<8*25; x++)
        cout("helloworld");
    cout("helloworld");
    log("This is my 2irst log.");

    start_user_program(mbinfo);
    
    while(1)
        ;
    return 0x12345678;
}