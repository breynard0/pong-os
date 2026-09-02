#include "idt.h"
#include "vga.h"

extern void main()
{
    init_idt();
    display_character_at_vga_buffer('8', 0);
    display_character_at_vga_buffer('7', 0);
    display_character_at_vga_buffer('2', 1);
    display_character_at_vga_buffer('4', 2);
}
