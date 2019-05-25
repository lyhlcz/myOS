build(){
    nasm -f bin -o $1.bin $1.asm
}

build char
build string
build a20
