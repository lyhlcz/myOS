build(){
    nasm -f bin -o $1.bin $1.asm
}

build bios-string
build vga-char
build vga-clear
build vga-string
