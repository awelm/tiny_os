#ifndef INCLUDE_INTERRUPTS
#define INCLUDE_INTERRUPTS

#include "pic.h"
#include "io.h"
#include "keyboard.h"

#define INTERRUPTS_DESCRIPTOR_COUNT 256 
#define INTERRUPTS_KEYBOARD 33 

struct IDT 
{
	unsigned short size;
	unsigned int address;
} __attribute__((packed));

struct IDTDescriptor {
	/* The lowest 32 bits */
	unsigned short offset_low; // offset bits 0..15
	unsigned short segment_selector; // a code segment selector in GDT or LDT

	/* The highest 32 bits */
	unsigned char reserved; // Just 0.
	unsigned char type_and_attr; // type and attributes
	unsigned short offset_high; // offset bits 16..31
} __attribute__((packed));

struct cpu_state {
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebp; 
	unsigned int esi; 
	unsigned int edi; 
} __attribute__((packed));

struct stack_state {
	unsigned int error_code;
	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
} __attribute__((packed));

struct IDT idt;
struct IDTDescriptor idt_descriptors[INTERRUPTS_DESCRIPTOR_COUNT];

void interrupts_init_descriptor(int index, unsigned int address)
{
	idt_descriptors[index].offset_high = (address >> 16) & 0xFFFF; // offset bits 0..15
	idt_descriptors[index].offset_low = (address & 0xFFFF); // offset bits 16..31

	idt_descriptors[index].segment_selector = 0x08; // The second (code) segment selector in GDT: one segment is 64b.
	idt_descriptors[index].reserved = 0x00; // Reserved.

	/*
	   Bit:     | 31              16 | 15 | 14 13 | 12 | 11     10 9 8   | 7 6 5 | 4 3 2 1 0 |
	   Content: | offset high        | P  | DPL   | S  | D and  GateType | 0 0 0 | reserved
		P	If the handler is present in memory or not (1 = present, 0 = not present). Set to 0 for unused interrupts or for Paging.
		DPL	Descriptor Privilige Level, the privilege level the handler can be called from (0, 1, 2, 3).
		S	Storage Segment. Set to 0 for interrupt gates.
		D	Size of gate, (1 = 32 bits, 0 = 16 bits).
	*/
	idt_descriptors[index].type_and_attr =	(0x01 << 7) | // P
						(0x00 << 6) | (0x00 << 5) |	      // DPL
						0xe;				              // 0b1110=0xE 32-bit interrupt gate
}

// defined in idt.s
void interrupt_handler_33();
void load_idt();

void interrupts_install_idt()
{
	interrupts_init_descriptor(INTERRUPTS_KEYBOARD, (unsigned int) interrupt_handler_33);

	idt.address = (unsigned int) &idt_descriptors;
	idt.size = sizeof(struct IDTDescriptor) * INTERRUPTS_DESCRIPTOR_COUNT;
	load_idt((unsigned int) &idt);

	pic_remap(PIC_1_OFFSET, PIC_2_OFFSET);
}


void interrupt_handler(__attribute__((unused)) struct cpu_state cpu, unsigned int interrupt, __attribute__((unused)) struct stack_state stack)
{
    unsigned char scan_code;
	unsigned char ascii;

	switch (interrupt){

		case INTERRUPTS_KEYBOARD:
			scan_code = keyboard_read_scan_code();

			if (scan_code <= KEYBOARD_MAX_ASCII) {
				ascii = keyboard_scan_code_to_ascii[scan_code];
				serial_configure_baud_rate(SERIAL_COM1_BASE, 4);
				serial_configure_line(SERIAL_COM1_BASE);
				char str[1];
				str[0] = ascii;
                log("Received the following key from the keyboard:");
                log_buffer(str, 1);
			}
			pic_acknowledge(interrupt);
			break;

		default:
			break;
    }
}

#endif