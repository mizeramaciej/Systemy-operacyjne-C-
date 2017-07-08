all: server client

server: server.c
	gcc -Wall server.c -o server -lrt

client: client.c
	gcc -Wall client.c -o client -lrt

test: all 
	./main	

clean:
	rm -f *.a *.o *.so
