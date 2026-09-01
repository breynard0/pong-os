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

mov di, message2
call write_text_tty

hlt

write_text_serial:
    wts_loop:
    mov ah, 0x1 ; send 1 byte
    mov bx, message ; prep base register with address of character
    add bx, cx
    mov al, [bx]
    int 0x14

    inc cx
    cmp [bx], 0xA
    jne wts_loop

    ret

init_video:
    mov ah, 0
    mov al, 2
    int 0x10
    ret

write_text_tty:
    mov cx, 0 ; init counter
    wtt_loop:
        mov bx, di ; set starting index
        add bx, cx ; add loop index
        mov ah, 0xE ; write TTY
        mov al, [bx] ; set chracter
        int 0x10

        inc cx ; increment counter
        cmp [bx], 0xA
        jne wtt_loop
    ret