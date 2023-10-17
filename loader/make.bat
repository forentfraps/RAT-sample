nasm -f win64 ../crypto/fast.asm -o fast.o
nasm -f win64 tricks.asm -o tricks.o
gcc -c ../crypto/aes.c -Ofast -nostdlib -w -static
gcc -c payload.c -ffreestanding -nostdlib -static
python ./encrypter.py
gcc -o loader.exe loader.c aes.o fast.o tricks.o -lwininet -Wint-conversion payload.o
del *.o