#include "keyboard.h"

#define QWERTY_KEY(x, i) {x, i},

struct KeyShiftPair
{
    char lower;
    char upper;
};

constexpr int SHIFT_PAIRS_LEN = 21;
static const struct KeyShiftPair shift_pairs[SHIFT_PAIRS_LEN] = {
    {'`', '~'},
    {'1', '!'},
    {'2', '@'},
    {'3', '#'},
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
    {'-', '_'},
    {'=', '+'},
    {'[', '{'},
    {']', '}'},
    {'\\', '|'},
    {';', ':'},
    {'\'', '\"'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
};

struct KeyMapping
{
    char ascii_character;
    uint8_t mod;
};

static const struct KeyMapping mappings[256] = {
    0,
    0,
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
    QWERTY_KEY('S', 0x04) // left shift
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
    QWERTY_KEY('?', 0x04) // right shift
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
    QWERTY_KEY('?', 0x30) // num lock
    QWERTY_KEY('?', 0x40) // scroll lock
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
};

char scancode_to_char(uint8_t scancode)
{
    static uint8_t modifiers = 0;

    uint8_t pressed = (scancode & 0x80) == 0;

    int inner_sc = scancode;
    if (!pressed)
    {
        inner_sc -= 0x80;
    }

    struct KeyMapping key_mapping = mappings[inner_sc];
    unsigned char out = key_mapping.ascii_character;
    uint8_t mod = key_mapping.mod;

    if (mod != 0)
    {
        if ((mod & 0x20) != 0)
        {
            // function keys are unimplemented
            return mod - 0x20;
        }

        // caps lock
        if (mod == 0x10)
        {
            if (pressed)
            {
                if ((modifiers & 0x10) != 0)
                {
                    modifiers &= ~0x10;
                }
                else
                {
                    modifiers |= 0x10;
                }
            }
            return 0;
        }

        switch (pressed)
        {
        case 0:
            // release
            modifiers &= ~mod;
            break;
        case 1:
        default:
            // press
            modifiers |= mod;
            break;
        }
        return 0;
    }

    // shift selection
    if ((modifiers & 0x04) != 0 || (modifiers & 0x10) != 0)
    {
        if (out >= 'a' && out <= 'z')
        {
            // pull the 6th bit low for letters
            out &= 0b11011111;
        }
        else
        {
            for (int i = 0; i < SHIFT_PAIRS_LEN; i++)
            {
                if (shift_pairs[i].lower == out)
                {
                    out = shift_pairs[i].upper;
                    break;
                }
            }
        }
    }

    // exclude anything releasing
    if (pressed == 0)
    {
        return 0;
    }

    return out;
}
