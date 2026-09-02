#include "vga.h"

#include "io.h"

void display_character_at_vga_buffer(const char c, int pos)
{
    char* vga_buffer = (char*) 0xb8000;

    *(vga_buffer + 2 * pos) = c;
}

void set_cursor_pos(const uint32_t pos)
{
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}