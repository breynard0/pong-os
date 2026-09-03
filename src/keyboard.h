#ifndef PONG_OS_KEYBOARD_H
#define PONG_OS_KEYBOARD_H
#include <stdint.h>
extern volatile uint8_t left_down;
extern volatile uint8_t right_down;
char scancode_to_char(uint8_t scancode);
#endif //PONG_OS_KEYBOARD_H
