all: server client


server: server.c
	gcc -Wall server.c -o server -lpthread

client: client.c
	gcc -Wall client.c -o client  -lpthread

test: all 
	./main	

clean:
	rm -f *.a *.o *.so
