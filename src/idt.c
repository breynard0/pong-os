#include "idt.h"
#include "io.h"
#include "keyboard.h"
#include "pong.h"

#include "vga.h"

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI     0x20

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */

static void PIC_sendEOI(uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_COMMAND,PIC_EOI);

    outb(PIC1_COMMAND,PIC_EOI);
}

static void PIC_remap(int offset1, int offset2)
{
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // unmask and let the world bask in the glory of my PICs
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

struct InterruptDescriptor32 IDT[256] = {0};
struct IDTDescriptor32 idt_descriptor = {};

struct interrupt_frame;

__attribute__((interrupt)) static void general_protection_fault(struct interrupt_frame* frame, unsigned long error_code)
{
    static int digit = 0;
    // display_character_at_vga_buffer(digit + '0', 0);
    digit++;
    if (digit > 9)
    {
        digit = 0;
    }
}

struct interrupt_frame;

__attribute__((interrupt)) static void keyboard_interrupt(struct interrupt_frame* frame)
{
    static uint32_t cursor = 0;
    const uint8_t sc = inb(0x60);
    char letter = scancode_to_char(sc);
    if (letter == 0x08) // backspace checking
    {
        if (cursor > 0) cursor--;
        display_character_at_vga_buffer(0, cursor);
    }
    else if (letter != 0)
    {
        display_character_at_vga_buffer(letter, cursor);
        cursor++;
    }
    set_cursor_pos(cursor);

    PIC_sendEOI(1);
}

volatile uint32_t counter = 0;

struct interrupt_frame;

__attribute__((interrupt)) static void timer_interrupt(struct interrupt_frame* frame)
{
    counter++;
    if (counter >= 60) counter = 0;
    PIC_sendEOI(0);
}

static void init_interrupt_gate(const uint8_t idt_pos, void* handler)
{
    struct InterruptDescriptor32* descriptor = &IDT[idt_pos];
    const uint32_t handler_addr = (uint32_t)handler;
    descriptor->offset_1 = handler_addr & 0x0000ffff;
    descriptor->offset_2 = (handler_addr & 0xffff0000) >> (4 * 4);
    descriptor->type_attributes = 0b10001110;
    descriptor->zero = 0;
    descriptor->selector = 0x08;
}

void init_idt()
{
    PIC_remap(0x20, 0x28);

    // Initialize General Protection fault
    init_interrupt_gate(0x0D, general_protection_fault);

    // Initialize Timer interrupt
    init_interrupt_gate(0x20, timer_interrupt);

    // Initialize Keyboard interrupt
    init_interrupt_gate(0x21, keyboard_interrupt);

    // Load IDT
    idt_descriptor.offset = (uint32_t)(void*)&IDT;
    idt_descriptor.size = sizeof(IDT) - 1;
    __asm__ __volatile__("lidt %0"
        :
        : "m" (idt_descriptor)
        : );

    // Re-enable interrupts
    __asm__ __volatile__("sti");
}
