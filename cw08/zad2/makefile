all: testsig testdiv


testdiv: testdiv.c
	gcc -Wall testdiv.c -o testdiv -lpthread

testsig: testsig.c
	gcc -Wall testsig.c -o testsig -lpthread

clean:
	rm -f *.a *.o *.so
