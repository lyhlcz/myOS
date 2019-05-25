    org     0x7c00
    [bits 16]
    ; First, BIOS loads the bootsector into 0000:7c00
    mov     ax, 0x1301
    mov     bx, 0x000c
    mov     cx, 5
    mov     dx, 0x0808
    mov     bp, msg
    int     0x10
    jmp     $

msg:
    db      "HELLO", 0
    times   510-($-$$) db 0
    dw      0xAA55
	