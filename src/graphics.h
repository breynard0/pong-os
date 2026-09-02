#ifndef PONG_OS_GRAPHICS_H
#define PONG_OS_GRAPHICS_H
#include <stdint.h>
extern const int WIDTH;
extern const int HEIGHT;
enum COLOURS
{
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHT_GRAY,
    DARK_GRAY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    YELLOW,
    WHITE
};
void put_pixel(uint32_t x, uint32_t y, uint8_t colour);
void draw_rect(uint32_t x, uint32_t y,  uint32_t width, uint32_t height, uint8_t colour);
#endif //PONG_OS_GRAPHICS_H
