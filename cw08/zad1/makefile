all: generator main


generator: generator.c
	gcc -Wall generator.c -o generator

main: main.c
	gcc -Wall main.c -o main -lpthread


test: all 
	./main	

clean:
	rm -f *.a *.o *.so
