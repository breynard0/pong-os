#include "keyboard.h"

#define QWERTY_KEY(x, i) {int idx = __COUNTER__ + 1; if (scancode == idx) { out=x;pressed=1; }else if (scancode == idx + 0x80) {out = x; pressed = 0; } mod=i;}

char scancode_to_char(uint8_t scancode)
{
    unsigned char out = '?';
    uint8_t mod = 0;
    uint8_t pressed = 0;
    QWERTY_KEY('\x1B', 0)
    QWERTY_KEY('1', 0)
    QWERTY_KEY('2', 0)
    QWERTY_KEY('3', 0)
    QWERTY_KEY('4', 0)
    QWERTY_KEY('5', 0)
    QWERTY_KEY('6', 0)
    QWERTY_KEY('7', 0)
    QWERTY_KEY('8', 0)
    QWERTY_KEY('9', 0)
    QWERTY_KEY('0', 0)
    QWERTY_KEY('-', 0)
    QWERTY_KEY('=', 0)
    QWERTY_KEY(0x08, 0) // backspace
    QWERTY_KEY(0x09, 0) // tab
    QWERTY_KEY('q', 0)
    QWERTY_KEY('w', 0)
    QWERTY_KEY('e', 0)
    QWERTY_KEY('r', 0)
    QWERTY_KEY('t', 0)
    QWERTY_KEY('y', 0)
    QWERTY_KEY('u', 0)
    QWERTY_KEY('i', 0)
    QWERTY_KEY('o', 0)
    QWERTY_KEY('p', 0)
    QWERTY_KEY('[', 0)
    QWERTY_KEY(']', 0)
    QWERTY_KEY('\n', 0)
    QWERTY_KEY('?', 0x02) // left control
    QWERTY_KEY('a', 0)
    QWERTY_KEY('s', 0)
    QWERTY_KEY('d', 0)
    QWERTY_KEY('f', 0)
    QWERTY_KEY('g', 0)
    QWERTY_KEY('h', 0)
    QWERTY_KEY('j', 0)
    QWERTY_KEY('k', 0)
    QWERTY_KEY('l', 0)
    QWERTY_KEY(';', 0)
    QWERTY_KEY('\'', 0)
    QWERTY_KEY('`', 0)
    QWERTY_KEY('?', 0x04) // left shift
    QWERTY_KEY('\\', 0)
    QWERTY_KEY('z', 0)
    QWERTY_KEY('x', 0)
    QWERTY_KEY('c', 0)
    QWERTY_KEY('v', 0)
    QWERTY_KEY('b', 0)
    QWERTY_KEY('n', 0)
    QWERTY_KEY('m', 0)
    QWERTY_KEY(',', 0)
    QWERTY_KEY('.', 0)
    QWERTY_KEY('/', 0)
    QWERTY_KEY('?', 0x05) // right shift
    QWERTY_KEY('*', 0)
    QWERTY_KEY('?', 0x08) // left alt
    QWERTY_KEY(' ', 0)
    QWERTY_KEY('?', 0x10) // caps lock
    QWERTY_KEY('1', 0x21) // f1
    QWERTY_KEY('2', 0x22) // f2
    QWERTY_KEY('3', 0x23) // f3
    QWERTY_KEY('4', 0x24) // f4
    QWERTY_KEY('5', 0x25) // f5
    QWERTY_KEY('6', 0x26) // f6
    QWERTY_KEY('7', 0x27) // f7
    QWERTY_KEY('8', 0x28) // f8
    QWERTY_KEY('9', 0x29) // f9
    QWERTY_KEY('0', 0x20) // f10
    QWERTY_KEY('?', 0x11) // num lock
    QWERTY_KEY('?', 0x12) // scroll lock
    QWERTY_KEY('7', 0)
    QWERTY_KEY('8', 0)
    QWERTY_KEY('9', 0)
    QWERTY_KEY('-', 0)
    QWERTY_KEY('4', 0)
    QWERTY_KEY('5', 0)
    QWERTY_KEY('6', 0)
    QWERTY_KEY('+', 0)
    QWERTY_KEY('1', 0)
    QWERTY_KEY('2', 0)
    QWERTY_KEY('3', 0)
    QWERTY_KEY('0', 0)
    QWERTY_KEY('.', 0)

    // TODO: finish keyboard driver with modifiers and also things not in main list

    // for now, exclude anything releasing
    if (pressed == 0)
    {
        return 0;
    }

    return out;
}
