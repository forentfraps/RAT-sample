nasm -f win64 ../crypto/fast.asm -o fast.o
nasm -f win64 tricks.asm -o tricks.o
gcc -c ../crypto/aes.c -Ofast -nostdlib -w -static
gcc -c payload.c -ffreestanding -nostdlib -static
gcc -c  windows_intern.c -static -ffreestanding
python ./encrypter.py
gcc -o loader.exe loader.c aes.o fast.o tricks.o windows_intern.o -O3 -s -nostartfiles -static -Wl,-e WinMainCRTStartup 
python ./entropy.py
del *.o