#include "graphics.h"

#include <stdint.h>

#include "io.h"

static const void* VGA_BASE_ADDRESS = (void*)0xA0000;

const int WIDTH = 640;
const int HEIGHT = 480;

static void select_map_mask(uint8_t mask)
{
    outb(0x3C4, 0x02);
    outb(0x3C5, mask);
}

static void read_map_select(uint8_t plane)
{
    outb(0x3CE, 0x04);
    outb(0x3CF, plane);
}

static uint8_t read_video_byte(const uint32_t byte_offset, const uint8_t plane)
{
    read_map_select(plane);
    uint8_t* location = (uint8_t*)VGA_BASE_ADDRESS + byte_offset;
    return *location;
}

static void single_8_channel(const uint32_t byte_offset, uint8_t buffer[8], uint8_t plane)
{
    uint8_t p_byte = read_video_byte(byte_offset, plane);
    for (int i = 0; i < 8; i++)
    {
        uint8_t bit = (p_byte & (1 << i)) != 0;
        if (bit)
        {
            buffer[i] |= 1 << plane;
        }
        else
        {
            buffer[i] &= ~(1 << plane);
        }
    }
}

static void read_video_bytes_8_channel(const uint32_t byte_offset, uint8_t buffer[8])
{
    single_8_channel(byte_offset, buffer, 0);
    single_8_channel(byte_offset, buffer, 1);
    single_8_channel(byte_offset, buffer, 2);
    single_8_channel(byte_offset, buffer, 3);
}

static uint8_t make_from_bits(const uint8_t bit_values[8], uint8_t idx)
{
    uint8_t out = 0;
    for (int i = 0; i < 8; i++)
    {
        uint8_t cur_bit_byte = bit_values[i];
        uint8_t cur_bit_value = (cur_bit_byte & (0x1 << idx)) != 0;
        out |= cur_bit_value << i;
    }
    return out;
}

static void write_video_byte(const uint32_t byte_offset, const uint8_t bit_values[8])
{
    uint8_t* location = (uint8_t*)VGA_BASE_ADDRESS + byte_offset;
    for (int plane_idx = 0; plane_idx < 4; plane_idx++)
    {
        select_map_mask(1 << plane_idx);
        uint8_t plane_channel_byte = make_from_bits(bit_values, plane_idx);
        *location = plane_channel_byte;
    }
}

static uint32_t compute_byte_offset(const uint32_t byte_x, const uint32_t byte_y)
{
    return byte_y * WIDTH / 8 + byte_x;
}

void put_pixel(const uint32_t x, const uint32_t y, const uint8_t colour)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

    uint8_t colours[8] = {};
    read_video_bytes_8_channel(compute_byte_offset(x / 8, y), colours);
    colours[x % 8] = colour;
    write_video_byte(compute_byte_offset(x / 8, y), colours);
}

void draw_rect(const uint32_t x, const uint32_t y, uint32_t width, const uint32_t height, const uint8_t colour)
{
    uint32_t end_x = x + width;
    uint32_t end_y = y + height;

    uint32_t byte_offset = compute_byte_offset(x / 8, y);

    uint8_t colours_left[8] = {};

    uint8_t colours_full[8] = {colour, colour, colour, colour, colour, colour, colour, colour};

    for (int draw_y = y; draw_y < end_y; draw_y++)
    {
        for (int draw_x = x / 8; draw_x <= end_x / 8; draw_x++)
        {
            if (draw_x == x / 8)
            {
                read_video_bytes_8_channel(compute_byte_offset(draw_x, draw_y), colours_left);
                for (int i = 8 - x % 8; i < 8; i++)
                {
                    colours_left[i] = colour;
                }
                write_video_byte(byte_offset, colours_left);
            }
            else if (draw_x == end_x / 8)
            {
                uint8_t colours_right[8] = {};
                read_video_bytes_8_channel(compute_byte_offset(draw_x, draw_y), colours_right);
                for (int i = 0; i < end_x % 8; i++)
                {
                    colours_right[i] = colour;
                }
                write_video_byte(byte_offset, colours_right);
            }
            else
            {
                write_video_byte(byte_offset, colours_full);
            }

            byte_offset++;
        }
        byte_offset = compute_byte_offset(x / 8, draw_y + 1);
    }
}
