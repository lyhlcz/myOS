%define CR0_PE (1 << 0)
%define CR0_MP (1 << 1)
%define CR0_EM (1 << 2)
%define CR0_TS (1 << 3)
%define CR0_WP (1 << 16)
%define CR0_PG (1 << 31)

%define PTE_PRESENT (1 << 0)
%define PTE_WRITE (1 << 1)
%define OUTTER_PGTBL 0x1000
%define INNER_PGTBL 0x2000

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

    ;Initialize inner page table
    mov     eax, PTE_WRITE|PTE_PRESENT
    mov     edi, INNER_PGTBL
    mov     ecx, 1024
    cld
init_pte:
    stosd
    add     eax, 4096
    loop    init_pte

    ;Initialize outter page table
    xor     eax, eax
    mov     edi, OUTTER_PGTBL
    mov     ecx, 1024
    cld
    rep
    stosd
    mov     dword [OUTTER_PGTBL+0*4], INNER_PGTBL|PTE_WRITE|PTE_PRESENT
    mov     dword [OUTTER_PGTBL+1*4], INNER_PGTBL|PTE_WRITE|PTE_PRESENT

    ;Load CR3
    mov     eax, OUTTER_PGTBL
    mov     cr3, eax

    ;Enable paging and write-protect
    mov     eax, cr0
    and     eax, ~(CR0_EM|CR0_TS)
    or      eax, CR0_PG|CR0_WP|CR0_MP
    mov     cr0, eax

    mov     al, 'X'
    mov     ah, 0x0c
    mov     [0xB8000], ax

    mov     al, 'Y'
    mov     ah, 0x0c
    mov     [0x4B8002], ax

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
	