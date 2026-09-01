BITS 16
ORG 0x7c00

jmp _start

section .data
startup_message: db "Hello, Hackers! Loading kernel...", 0xD, 0xA

gdt:
    dq 0x0000000000000000    ; null descriptor

    dw 0xffff                ; code limit low
    dw 0x0000                ; code base low
    db 0x00                  ; code base middle
    db 0b10011111            ; code access bytes
    db 0b11001111            ; code flags and limit high
    db 0x00                   ; code base high

    dw 0xffff                ; data limit low
    dw 0x0000                ; data base low
    db 0x00                  ; data base middle
    db 0b10010011            ; data access bytes
    db 0b11001111            ; data flags and limit high
    db 0x00                  ; data base high
gdt_end:

gdtr:
    dw gdt_end - gdt - 1
    dd gdt

KERNEL_OFFSET equ 0x1000
KERNEL_SECTOR_COUNT equ 0x80

boot_drive: db 0

section .text
global _start
_start:
    cli ; clear interrupts

    ; grab the CD's drive number
    mov [boot_drive], dl

    ; set stack to bootloader code
    mov ax, 0
    mov ss, ax
    mov sp, 0x7c00

    mov ah, 0x0 ; initialize serial
    mov dx, 0x0 ; set COM1
    int 0x14

    mov dl, 1
    call disk_load
    ; log startup message
    call log_startup

;     call load_kernel ; load my C code into memory


    call init_video ; set up VGA

    lgdt [gdtr] ; load gdt

    call activate_a20 ; enable A20 line

    call enter_protected ; protected mode!

log_startup:
    mov cx, 0
    log_startup_loop:
        mov bx, startup_message
        add bx, cx

        mov ah, 0x01
        mov al, [bx]
        mov dx, 0
        int 0x14

        inc cx
        cmp [bx], 0xA
        jne log_startup_loop
    ret

activate_a20:
    bits 16
    mov ax, 0x2403
    int 0x15
    ret

disk_load:
    ; input is zero indexed sector, convert to one-indexed then set cl
    inc dl
    mov cl, dl

    mov ah, 0x02
    mov dl, [boot_drive] ; load drive
    mov ch, 0 ; cylinder 0
    mov dh, 0, ; first side
    mov al, 1 ; read 1 sector

    ; set target address, schema is (ES << 4) + bx
    mov bx, 0
    mov es, bx
    mov bx, KERNEL_OFFSET
    int 0x13
    ret

init_video:
    mov ah, 0
    mov al, 0x2
    int 0x10
    ret

enter_protected:
    cli
    mov eax, cr0
    or al, 1 ; set protection enable bit
    mov cr0, eax
    jmp 0x08:pmode_main

bits 32
pmode_main:
    ; initialize all registers to point at data segment
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x900000
    mov esp, ebp

    call load_kernel ; In search of greater pastures, we now go to C land and will not return

load_kernel:
    call KERNEL_OFFSET
    jmp $
