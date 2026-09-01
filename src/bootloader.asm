org 0x7c00 ; boot sectors are loaded here

section .data
message: db "Hack Club says hello!", 0xD, 0xA
message2: db "Yippee!", 0xD, 0xA

section .text

cli ; clear interrupts

mov ah, 0x0 ; initialize serial
mov dx, 0x0 ; set COM1
int 0x14

call init_video

mov ax, 0xB800
mov gs, ax
mov byte [gs:0x0], 'H'
mov byte [gs:0x1], 0x0f
mov byte [gs:0x2], 'i'
mov byte [gs:0x3], 0x0f

; call print_four

hlt


init_video:
    mov ah, 0
    mov al, 0x2
    int 0x10
    ret

print_four:
    mov ah, 0x01
    mov al, '4'
    int 0x14
    ret