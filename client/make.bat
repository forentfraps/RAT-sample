clang -c utils.c --target=x86_64-w64-windows-gnu
clang -c tcp.c --target=x86_64-w64-windows-gnu
clang -c udp.c --target=x86_64-w64-windows-gnu
clang -c os.c --target=x86_64-w64-windows-gnu
clang -c win_client.c --target=x86_64-w64-windows-gnu
clang -o win_client.exe win_client.o os.o tcp.o udp.o utils.o -lws2_32 -lMswsock -lAdvApi32 --target=x86_64-w64-windows-gnu -g
del *.o
