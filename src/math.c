#include "math.h"

#include <stdint.h>

uint32_t square_root(uint32_t square)
{
    if (square == 0) return 0;
    if (square == 1) return 1;

    const int ITERATIONS = 16;

    uint32_t guess = square / 2;
    for (int i = 0; i < ITERATIONS; i++)
    {
        uint32_t bound = square / guess;
        guess = (bound + guess) / 2;
    }
    return guess;
}

uint32_t distance(uint32_t x, uint32_t y)
{
    return square_root(x * x + y * y);
}

uint32_t absolute(const int32_t x)
{
    if (x < 0)
    {
        return -x;
    }
    return x;
}

int32_t maximum(const int32_t a, const int32_t b)
{
    return a > b ? a : b;
}

int32_t minimum(const int32_t a, const int32_t b)
{
    return a < b ? a : b;
}