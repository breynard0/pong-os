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

; mov di, message2
; call write_text_tty

mov dx, 4
call write_digit_tty
mov dx, 5
call write_digit_tty
call write_newline
mov dx, 181
call write_byte_tty
call write_newline

hlt


init_video:
    mov ah, 0
    mov al, 0x2
    int 0x10
    ret

write_newline:
    mov ah, 0xE ; write tty
    mov al, 0xD
    int 0x10
    mov al, 0xA
    int 0x10
    ret

write_digit_tty:
    mov ah, 0xE ; write tty
    add dx, '0' ; convert to ASCII
    mov al, dl ; print number
    int 0x10
    ret

write_byte_tty:
    mov ax, dx
    mov dx, 0
    mov cx, 100
    div cx
    mov cx, dx
    mov dx, ax
    call write_digit_tty
    mov ax, cx
    mov dx, 0
    mov cx, 10
    div cx
    mov cx, dx
    mov dx, ax
    call write_digit_tty
    mov dx, cx
    call write_digit_tty
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
