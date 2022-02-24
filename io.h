#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

/* Frame Buffer */

char *fb = (char *) 0x000B8000; // Frame Buffer start address
#define FB_CHAR_LEN 80*25 // frame buffer character limit
unsigned int fb_char_index = 0; // Keep track of last written frame buffer character index

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

/**
  *  Sends the given data to the given I/O port. Defined in io.s
  *
  *  @param port The I/O port to send the data to
  *  @param data The data to send to the I/O port
*/
void outb(unsigned short port, unsigned char data);

/**
  *  Read a byte from an I/O port. Defined in io.s
  *
  *  @param  port The address of the I/O port
  *  @return      The read byte
*/
unsigned char inb(unsigned short port);

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

void fb_write_char(unsigned int char_index, char c, unsigned char fg, unsigned char bg) {
   fb_write_cell(char_index * 2, c, fg, bg);
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

void clear() {
    for (unsigned int i = 0; i < FB_CHAR_LEN; i++)
        fb_write_char(i,0,0,0);
    fb_char_index = 0;
}

/**
  * Write a string to the framebuffer and automatically advance the cursor and
  * scroll screen if necessary. Old output won't be visible anymore
*/
int write(char *buf, unsigned int len) {
    // clear screen if overflow detected
    if(fb_char_index + len > FB_CHAR_LEN)
        clear();
    for (unsigned int i = 0; i < len; i++) {
        fb_write_char(fb_char_index + i, buf[i], FB_WHITE, FB_BLACK);
    }
    fb_char_index += len;
    fb_move_cursor(fb_char_index);
    return len;
}

unsigned int strlen(char *c_str) {
    int len = 0;
    while(c_str[len] != '\0')
        len++;
    return len;
}

// Write C string to framebuffer
int cout(char *c_str) {
    return write(c_str, strlen(c_str));
}

/* IO ports */

/* All the I/O ports are calculated relative to the data port. This is because
 * all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
 * order, but they start at different values.
*/

#define SERIAL_COM1_BASE                0x3F8      /* COM1 (aka serial port #1) base port */

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* The I/O port commands */

/* 
 * Tells the serial port to expect the highest 8 bits on the data port,
 * followed by another call with the lowest 8 bits
*/
#define SERIAL_LINE_ENABLE_DLAB         0x80

/**
 *  Sets the speed of the data being sent. The default speed of a serial
 *  port is 115200 bits/s. The argument is a divisor of that number, hence
 *  the resulting speed becomes (115200 / divisor) bits/s.
 *
 *  @param com      The COM port to configure
 *  @param divisor  The divisor
*/
void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
    outb(SERIAL_LINE_COMMAND_PORT(com),
            SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(com),
            (divisor >> 8) & 0x00FF);
    outb(SERIAL_DATA_PORT(com),
            divisor & 0x00FF);
}

/** 
  *  Configures the line of the given serial port. The port is set to have a
  *  data length of 8 bits, no parity bits, one stop bit and break control
  *  disabled.
  *
  *  @param com The serial port to configure
*/
void serial_configure_line(unsigned short com)
{
    /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     * Content: | d | b | prty  | s | dl  |
     * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
    */
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

/**
  * Enables FIFO, clears both receiver and transmission FIFO queues, and
  * uses 14 bytes as size of queue
*/
void serial_configure_fifo(unsigned short com)
{
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

/**
  * Disable interrupts and enable RTS and DTR 
*/
void serial_configure_modem(unsigned short com)
{
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

/**
  * Master function to configure all aspects of the
  * serial port.
*/
void configure_serial_port(unsigned short serial_port) {
    serial_configure_baud_rate(serial_port, 1);
    serial_configure_line(serial_port);
    serial_configure_fifo(serial_port);
    serial_configure_modem(serial_port);
}

/**
  *  Checks whether the transmit FIFO queue is empty or not for the given COM
  *  port.
  *
  *  @param  com The COM port
  *  @return 0 if the transmit FIFO queue is not empty
  *          1 if the transmit FIFO queue is empty
*/
int serial_is_transmit_fifo_empty(unsigned int com)
{
    /* 0x20 = 0010 0000 */
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

// Write a byte to the specified serial port
void write_to_serial_port(unsigned int com, char data) {
    while(serial_is_transmit_fifo_empty(com) == 0)
        ;
    outb(SERIAL_DATA_PORT(com), data);
}

void log_buffer(char *buf, unsigned int len) {
    unsigned int com = SERIAL_COM1_BASE;
    for(unsigned int i=0; i<len; i++)
        write_to_serial_port(com, buf[i]); 
    write_to_serial_port(com, '\n');
    // spin until data is flushed
    while(serial_is_transmit_fifo_empty(com) == 0)
        ;
}

void log(char *c_str) {
    log_buffer(c_str, strlen(c_str));
}

#endif /* INCLUDE_IO_H */