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

    mov     ah, 0x0c
    mov     al, 'X'
    mov     [es:0], ax
stop:
    jmp     stop

    times   510-($-$$) db 0
    dw      0xAA55
	