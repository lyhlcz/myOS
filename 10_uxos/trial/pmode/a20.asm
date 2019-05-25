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

    ;Enable A20
wait_8042_1:
    in      al, 0x64
    test    al, 0x2
    jnz     wait_8042_1
    mov     al, 0xd1
    out     0x64, al

wait_8042_2:
    in      al, 0x64
    test    al, 0x2
    jnz     wait_8042_2
    mov     al, 0xdf
    out     0x60, al

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

    mov     bl, 'Y'
    mov     bh, 0x0c
    mov     [0x100000], bx
    mov     ax, [0x100000]
    mov     [0xB8000], ax

    jmp     $

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
	