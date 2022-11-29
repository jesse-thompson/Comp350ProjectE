dd if=/dev/zero of=diskc.img bs=512 count=256
nasm bootload.asm
dd if=bootload of=diskc.img conv=notrunc

bcc -c -ansi -o kernelc.o kernel.c
as86 kernel.asm -o kernelasm.o
ld86 -d -o kernel kernelc.o kernelasm.o

bcc -c -ansi -o shellc.o shell.c
as86 userlib.asm -o userlibasm.o
ld86 -d -o shell shellc.o userlibasm.o

bcc -ansi -c -o tstpr1.o tstpr1.c
as86 -o userlib.o userlib.asm
ld86 -d -o tstpr1 tstpr1.o userlib.o

bcc -ansi -c -o tstpr2.o tstpr2.c
as86 -o userlib.o userlib.asm
ld86 -d -o tstpr2 tstpr2.o userlib.o

gcc -o loadFile loadFile.c

./loadFile kernel
./loadFile shell
./loadFile message.txt
./loadFile dirtst.txt
./loadFile tstpr1
./loadFile tstpr2

