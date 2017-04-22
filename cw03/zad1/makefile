all: main ddd

main: main.c
	gcc -Wall main.c -o main
	
test: all
	./main test
	
ddd:
	gcc -Wall ddd.c -o ddd

clean:
	rm -f *.a *.o *.so *.txt