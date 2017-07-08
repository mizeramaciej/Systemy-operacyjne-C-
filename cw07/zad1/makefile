all: libfifo libhelpers barber breeder

libfifo: fifo.c
	gcc -c -Wall fifo.c -o $@.o
	ar -rcs $@.a $@.o
	
libhelpers: helpers.c
	gcc -c -Wall helpers.c -o $@.o
	ar -rcs $@.a $@.o

barber: barber.c
	gcc -Wall barber.c -o barber -L. -lhelpers -lfifo

breeder: breeder.c
	gcc -Wall breeder.c -o breeder -L. -lhelpers -lfifo



test: all 
	./main	

clean:
	rm -f *.a *.o *.so
