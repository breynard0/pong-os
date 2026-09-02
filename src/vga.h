#ifndef PONG_OS_VGA_H
#define PONG_OS_VGA_H
#include <stdint.h>
void display_character_at_vga_buffer(const char c, int pos);
void set_cursor_pos(uint32_t pos);
#endif //PONG_OS_VGA_H
