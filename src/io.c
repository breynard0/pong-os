#include <stdint.h>

void outb(uint16_t port, uint8_t data)
{
    __asm__ __volatile__ ("outb %0, %1"
        :
        : "a" (data), "Nd" (port)
        :
    );
}

uint8_t inb(uint16_t port)
{
    uint8_t out;
    __asm__ __volatile__ (
            "inb %1, %0"\
            : "=a" (out)
            : "Nd" (port)
        );
    return out;
}