org 0x7c00 ; boot sectors are loaded here

section .data
message: db "Hack Club says hi!", 0xD, 0xA

section .text

cli ; clear interrupts

mov ah, 0x0 ; initialize serial
mov dx, 0x0 ; set COM1
int 0x14

loop:
mov ah, 0x1 ; send 1 byte
mov bx, message ; prep base register with address of character
add bx, cx
mov al, [bx]
int 0x14

inc cx
cmp [bx], 0xA
jne loop

hlt