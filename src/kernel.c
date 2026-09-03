#include "idt.h"
#include "io.h"
#include "keyboard.h"
#include "pong.h"

// to give roughly 60 Hz
static constexpr uint32_t PIT_COUNT = 19886;

static void init_pit()
{
    __asm__ __volatile__("cli");
    outb(0x40, PIT_COUNT & 0xFF);
    outb(0x40, (PIT_COUNT & 0xFF00) >> 8);
    __asm__ __volatile__("sti");
    return;
}

#ifndef VIDEO_MODE
#define VIDEO_MODE 0x12
#endif

extern void kmain()
{
    init_idt();

    if (VIDEO_MODE != 0x12) return;

    init_pit();

    init_pong();
    uint32_t last_counter = 0;
    while (true)
    {
        if (counter != last_counter)
        {
            uint32_t paddle_input = 0;

            if (right_down) paddle_input = 1;
            if (left_down) paddle_input = 2;
            if (left_down && right_down) paddle_input = 0;

            pong_tick(paddle_input);
            last_counter = counter;
        }
    }
}
