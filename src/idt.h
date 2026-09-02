#ifndef PONG_OS_IDT_H
#define PONG_OS_IDT_H
#include <stdint.h>

struct __attribute__((packed)) InterruptDescriptor32
{
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero; // unused, set to 0
    uint8_t type_attributes; // gate type, dpl, and p fields
    uint16_t offset_2; // offset bits 16..31
};

extern struct InterruptDescriptor32 IDT[256];

struct __attribute__((packed)) IDTDescriptor32
{
    uint16_t size;
    uint32_t offset;
};

extern struct IDTDescriptor32 idt_descriptor;

void init_idt();

#endif //PONG_OS_IDT_H
