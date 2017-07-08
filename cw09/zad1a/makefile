all: main


main: main.c
	gcc -Wall main.c -o main -lpthread

test: all 
	./main	

clean:
	rm -f *.a *.o *.so
