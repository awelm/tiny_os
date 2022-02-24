#ifndef MULTIBOOT_H
#define MULTIBOOT_H
// The following was copied from https://www.gnu.org/software/grub/manual/multiboot/html_node/multiboot.h.html

/* The symbol table for a.out. */
typedef struct aout_symbol_table
{
 unsigned long tabsize;
 unsigned long strsize;
 unsigned long addr;
 unsigned long reserved;
} aout_symbol_table_t;

/* The section header table for ELF. */
typedef struct elf_section_header_table
{
 unsigned long num;
 unsigned long size;
 unsigned long addr;
 unsigned long shndx;
} elf_section_header_table_t;

/* The Multiboot information. */
typedef struct multiboot_info
{
 unsigned long flags;
 unsigned long mem_lower;
 unsigned long mem_upper;
 unsigned long boot_device;
 unsigned long cmdline;
 unsigned long mods_count;
 unsigned long mods_addr;
 union
 {
   aout_symbol_table_t aout_sym;
   elf_section_header_table_t elf_sec;
 } u;
 unsigned long mmap_length;
 unsigned long mmap_addr;
} multiboot_info_t;

/* The module structure. */
typedef struct module
{
 unsigned long mod_start;
 unsigned long mod_end;
 unsigned long string;
 unsigned long reserved;
} module_t;

#endif /* MULTIBOOT_H */