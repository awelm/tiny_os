global gdt_flush    ; Allows the C code to call gdt_flush().

gdt_flush:
   mov eax, [esp+4]  ; Get the pointer to the GDT, passed as a parameter.
   lgdt [eax]        ; Load the GDT table into the processor

   mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment entry
   mov ds, ax        ; Load this value into all data segment selector registers
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   jmp 0x08:.flush_cs   ; 0x08 is the offset to our code segment entry in the GDT table.
                        ; We need to hardcode jump here to update the cs register

.flush_cs:
   ret