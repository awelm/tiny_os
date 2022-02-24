#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

// Frame Buffer start address
char *fb = (char *) 0x000B8000;

// Colors
#define FB_BLACK     0
#define FB_GREEN     2
#define FB_DARK_GREY 8
#define FB_WHITE     15

// Framebuffer I/O ports
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

// Framebuffer I/O port commands
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/** outb:
  *  Sends the given data to the given I/O port. Defined in io.s
  *
  *  @param port The I/O port to send the data to
  *  @param data The data to send to the I/O port
*/
void outb(unsigned short port, unsigned char data);

/**
  *  Writes a character with the given foreground and background to position i
  *  in the framebuffer.
  *
  *  @param i  The location in the framebuffer
  *  @param c  The character
  *  @param fg The foreground color
  *  @param bg The background color
*/
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

/**
  *  Moves the cursor of the framebuffer to the given position
  *
  *  @param pos The new position of the cursor
*/
void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

/**
  * Write a string to the framebuffer and automatically advance the cursor and
  * scroll screen if necessary. Old output won't be visible anymore
*/
int write(char *buf, unsigned int len) {
    unsigned int i;
    for (i = 0; i < len; i++) {
        fb_write_cell(i * 2, buf[i], FB_WHITE, FB_BLACK);
    }
    fb_move_cursor(i * 2);
    return len;
}

#endif /* INCLUDE_IO_H */