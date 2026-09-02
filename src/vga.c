#include "vga.h"

void display_character_at_vga_buffer(const char c, int pos)
{
    char* vga_buffer = (char*) 0xb8000;

    *(vga_buffer + 2 * pos) = c;
}