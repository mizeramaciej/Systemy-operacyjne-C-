CC=gcc
CFLAGS=-c
OUTPUT=tmpwynik.txt

# $@ means that output of the compilation will be put in the file named on the left side of the :
# the $< is the first item in the dependencies list, and the CFLAGS macro is defined as above

all: libgenerator libbsort libshuffler main

#creating static library

libgenerator: generator.c
	$(CC) $(CFLAGS) $^ -o $@.o
	ar -rcs $@.a $@.o

#crs options

libbsort: bsort.c
	$(CC) $(CFLAGS) $^ -o $@.o
	ar -rcs $@.a $@.o

libshuffler: shuffler.c
	$(CC) $(CFLAGS) $^ -o $@.o
	ar -rcs $@.a $@.o

main: main.c
	$(CC) -Wall $@.c -o $@.o -L. -lgenerator -lbsort -lshuffler


maintime: clean all maintime.c
	$(CC) -Wall $@.c -o $@.o -L. -lgenerator -lbsort -lshuffler
	echo "maintime\n" >> $(OUTPUT)
	./$@.o >> $(OUTPUT)


clean:
	rm -f *.a *.o *.so 

test1: clean all
	./main.o generate genFile 20 50

test2: clean all
	./main.o generate sufFile 10 1
	cat sufFile
	./main.o shuffle lib sufFile 10 1
	cat sufFile
	
test3: clean all
	./main.o generate sufFile 10 1
	cat sufFile
	./main.o shuffle sys sufFile 10 1
	cat sufFile
	
test4: clean all	
	./main.o generate genFile 20 1
	cat genFile
	./main.o sort lib genFile 20 1
	cat genFile
	
test5: clean all	
	./main.o generate genFile 10 1
	cat genFile
	./main.o sort sys genFile 10 1
	cat genFile	
	
