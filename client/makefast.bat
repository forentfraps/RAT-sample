gcc -c utils.c
gcc -c tcp.c
gcc -c udp.c
gcc -c os.c
gcc -c win_client.c
gcc -o win_client.exe win_client.o os.o tcp.o udp.o utils.o -lws2_32 -lMswsock -lAdvApi32 -Ofast -s
del *.o
