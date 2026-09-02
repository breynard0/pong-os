#include "idt.h"
#include "io.h"

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
    uint8_t sc = inb(0x60);
    if (sc == 0x04)
    {
        display_character_at_vga_buffer('3', 0);
    }
    PIC_sendEOI(1);
}

struct interrupt_frame;

__attribute__((interrupt)) static void timer_interrupt(struct interrupt_frame* frame)
{
}

void init_idt()
{
    PIC_remap(0x20, 0x28);

    {
        // Initialize General Protection fault
        struct InterruptDescriptor32* general_protection_descriptor = &IDT[0x0D];
        void* gp_handler = general_protection_fault;
        const uint32_t gp_handler_addr = (uint32_t)gp_handler;
        general_protection_descriptor->offset_1 = gp_handler_addr & 0x0000ffff;
        general_protection_descriptor->offset_2 = (gp_handler_addr & 0xffff0000) >> (4 * 4);
        general_protection_descriptor->type_attributes = 0b10001110;
        general_protection_descriptor->zero = 0;
        general_protection_descriptor->selector = 0x08;
    }

    {
        // Initialize Timer interrupt
        struct InterruptDescriptor32* timer_descriptor = &IDT[0x20];
        void* timer_handler = keyboard_interrupt;
        const uint32_t timer_handler_addr = (uint32_t)timer_handler;
        timer_descriptor->offset_1 = timer_handler_addr & 0x0000ffff;
        timer_descriptor->offset_2 = (timer_handler_addr & 0xffff0000) >> (4 * 4);
        timer_descriptor->type_attributes = 0b10001110;
        timer_descriptor->zero = 0;
        timer_descriptor->selector = 0x08;
    }

    {
        // Initialize Keyboard interrupt
        struct InterruptDescriptor32* keyboard_descriptor = &IDT[0x21];
        void* keyboard_handler = keyboard_interrupt;
        const uint32_t keyboard_handler_addr = (uint32_t)keyboard_handler;
        keyboard_descriptor->offset_1 = keyboard_handler_addr & 0x0000ffff;
        keyboard_descriptor->offset_2 = (keyboard_handler_addr & 0xffff0000) >> (4 * 4);
        keyboard_descriptor->type_attributes = 0b10001110;
        keyboard_descriptor->zero = 0;
        keyboard_descriptor->selector = 0x08;
    }

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
