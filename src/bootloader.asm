org 0x7c00 ; boot sectors are loaded here

section .data
message: db "Hack Club says hello!", 0xD, 0xA
message2: db "Yippee!", 0xD, 0xA

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

section .text
global _start
_start:
    cli ; clear interrupts

    mov ah, 0x0 ; initialize serial
    mov dx, 0x0 ; set COM1
    int 0x14

    lgdt [gdtr] ; load gdt

    ; enable A20 line


    ; call init_video
    ;
    ; mov ax, 0xB800
    ; mov gs, ax
    ; mov byte [gs:0x0], 'H'
    ; mov byte [gs:0x1], 0x0f
    ; mov byte [gs:0x2], 'i'
    ; mov byte [gs:0x3], 0x0f

hlt


init_video:
    mov ah, 0
    mov al, 0x2
    int 0x10
    ret