%define CR0_PE (1 << 0)
%define CR0_MP (1 << 1)
%define CR0_EM (1 << 2)
%define CR0_TS (1 << 3)
%define CR0_WP (1 << 16)
%define CR0_PG (1 << 31)

    org 0x7c00
    [bits 16]
    ;First, BIOS loads the bootsector into 0000:7c00.
    cli
    xor     ax, ax
    mov     ds, ax
    mov     ss, ax
    mov     sp, 0

    ;Switch to protect mode
    lgdt    [gdt_desc]
    mov     eax, cr0
    or      eax, CR0_PE
    mov     cr0, eax
    jmp     0x08:start32

    [bits 32]
start32:
    ; In protect mode
    cli
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     esp, 0x10000

    ; clear vga
    mov     ah, 0x00
    mov     al, ' '
    mov     ebx, 80*50*2
L:
    sub     ebx, 2
    mov     [ebx+0xB8000], ax
    jne     L

    ;write string
    mov     si, msg
    xor     ebx, ebx
    mov     ah, 0x0c
L2:
    mov     al, [si]
    cmp     al, 0
    je      stop
    mov     [0xB8000+ebx], ax
    inc     si
    add     ebx, 2
    jmp     L2

stop:
    jmp     $
msg:
    db "Hello world in protected mode", 0

    align   8
gdt:
    dw      0, 0, 0, 0  ;dummy

    dw      0xFFFF      ;limit=4GB
    dw      0x0000      ;base address=0
    dw      0x9A00      ;code read/exec
    dw      0x00CF      ;granularity=4096, 386

    dw      0xFFFF      ;limit=4GB
    dw      0x0000      ;base address=0;
    dw      0x9200      ;data read/write
    dw      0x00CF      ;granularity=4096, 386

    align   8
gdt_desc:
    dw      23          ;gdt limit=sizeof(gdt) - 1
    dw      gdt

    times   510-($-$$) db 0
    dw      0xAA55
