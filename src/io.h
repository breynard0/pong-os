#ifndef PONG_OS_IO_H
#define PONG_OS_IO_H
#include <stdint.h>

void outb(uint16_t port, uint8_t data);

uint8_t inb(uint16_t port);
#endif //PONG_OS_IO_H
