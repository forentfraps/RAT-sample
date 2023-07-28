all:
	gcc -c server.c -o server.o
	gcc -c netlib.c -o netlib.o
	gcc -c utils.c -o utils.o
	gcc -c db.c -o db.o
	gcc -o lnx_server server.o netlib.o db.o utils.o

	rm -rf *.o
	
