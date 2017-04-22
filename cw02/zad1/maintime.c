#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> //dla clock i CLOCK_PER_SEC
#include <sys/times.h> //dla times
#include "generator.h"
#include "bsort.h"
#include "shuffler.h"
#include <unistd.h>


void checkTimes(struct tms *prevTimes, clock_t *prevReal, struct tms *firstTimes, clock_t *firstReal);
void makeTest(int recordAmount,int recordSize);


	char *nameFile="genFile";
	char *copy="genCopy";
	char *finalCopy="KopiaGenFile";
	
	struct tms prevTimes;
    clock_t prevReal;
    struct tms firstTimes;
    clock_t firstReal;
    

int main(int argc,char **argv){
	
	printf("maintime START\n");

// rozmiary rocordu 4,512, 4096 i 8192 bajty. Dla każdego rozmiaru rekordu wykonaj dwa 
//	testy różniące się liczbą rekordów w sortowanym pliku


	prevTimes.tms_stime = -1;
	makeTest(1000,8192);
	makeTest(1500,4096);
	makeTest(2000,512);
	makeTest(3000,4);

	printf("maintime END\n");
return 0;
}

#define CLK sysconf(_SC_CLK_TCK)

void makeTest(int recordAmount,int recordSize){
	
	printf("------TEST------\n   Liczba rekordow= %d, rozmiar rekordu= %d\n\n",recordAmount,recordSize); 
	
	checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
	
	printf("Generating and coping\n");
	generateRecordsFile(nameFile,recordAmount,recordSize);
	copyFile(nameFile, copy, recordAmount,recordSize);
	copyFile(nameFile,finalCopy,recordAmount,recordSize);
	
	checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
	
	printf("System bubble sort\n");
	bSortSys(nameFile,recordAmount,recordSize);
	checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
	
	printf("Library bubbl sort\n");
	bSortLib(copy,recordAmount,recordSize);
	checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
	
	printf("Coping from generated file\n");
	copyFile(finalCopy, copy,recordAmount,recordSize);
	copyFile(finalCopy,nameFile,recordAmount,recordSize);
	checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
	
	printf("System shuffle\n");
	ShuffleSys(nameFile,recordAmount,recordSize);
	checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
	
	printf("Library shuffle\n");
	ShuffleLib(copy,recordAmount,recordSize);
	checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
	
}

void checkTimes(struct tms *prevTimes, clock_t *prevReal,
        struct tms *firstTimes, clock_t *firstReal) {

    struct tms now;
    times(&now);
    clock_t nowReal = clock();
    if (prevTimes->tms_stime == -1) {
        *firstTimes = now;
        *firstReal = nowReal;
    } else {
    	printf("\tFrom the previous check:\tR %.5f\tS %.5f\tU %.5f",
                ((double) (nowReal - *(prevReal))) / CLOCKS_PER_SEC,
                ((double) (now.tms_stime - prevTimes->tms_stime)) / CLK,
                ((double) (now.tms_utime - prevTimes->tms_utime)) / CLK);
    }
    printf("\n\tTime:\t\t\t\tR %.5f\tS %.5f\tU %.5f\n\n",
            ((double) nowReal) / CLOCKS_PER_SEC,
            ((double) now.tms_stime) / CLK,
            ((double) now.tms_utime) / CLK);
    *prevReal = nowReal;
    *prevTimes = now;
}