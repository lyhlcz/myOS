    org     0x7c00
    [bits 16]
    mov     ax, 0xb800
    mov     es, ax

    ; clear vga
    mov     ah, 0x00
    mov     al, ' '
    mov     bx, 80*50*2
L:
    sub     bx, 2
    mov     [es:bx], ax
    jne     L

    ;write string
    mov     si, msg
    xor     bx, bx
    mov     ah, 0x0c
L2:
    mov     al, [si]
    cmp     al, 0
    je      stop
    mov     [es:bx], ax
    inc     si
    add     bx, 2
    jmp     L2

stop:
    jmp     stop

msg:
    db      "HELLO", 0
    times   510-($-$$) db 0
    dw      0xAA55
	