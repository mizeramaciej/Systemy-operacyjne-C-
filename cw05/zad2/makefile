all: slave master

slave: slave.c
	gcc -Wall slave.c -o slave -lm
#lm linkint math library
master: master.c
	gcc -Wall master.c -o master	
	
test: all
	rm -f test
	./master test 600	

clean:
	rm -f *.a *.o *.so
	rm -f test data master slave
