#include "graphics.h"
#include "idt.h"
#include "vga.h"

extern void main()
{
    init_idt();

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            put_pixel(x, y, y % 16);
        }
    }

    draw_rect(160, HEIGHT / 4, WIDTH / 2, HEIGHT / 2, RED);
}
