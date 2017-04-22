#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>

#include "generator.h"
#include "bsort.h"
#include "shuffler.h"

int main(int argc,char **argv){
	
	printf("START\n");
	char *nameFile="generatedFile";
	int recordAmount =512;
	int recordSize=4096;
	
//
//	GENERATE
//	
	if(argc==5 && strcmp("generate",argv[1])==0){
		nameFile=argv[2];
		recordAmount=atoi(argv[3]);		
		recordSize=atoi(argv[4]);
		printf("GERERATING : FILE = %s , recordAmount = %d , recordSize = %d \n",nameFile,recordAmount,recordSize);

		//generate
		generateRecordsFile(nameFile,recordAmount,recordSize);
		return 0;
	}

//
//	SORT
//	
	
	if(argc==6 && strcmp("sort",argv[1])==0){
		nameFile=argv[3];
		recordAmount=atoi(argv[4]);		
		recordSize=atoi(argv[5]);
		if(strcmp("sys",argv[2])==0){
			printf("SYS SORTING : FILE = %s , recordAmount = %d , recordSize = %d \n",nameFile,recordAmount,recordSize);
			//sortowanie z uzyciem funkcji systemowych		
			bSortSys(nameFile,recordAmount,recordSize);
		}else if(strcmp("lib",argv[2])==0){
			printf("LIB SORTING : FILE = %s , recordAmount = %d , recordSize = %d \n",nameFile,recordAmount,recordSize);
			//sortowanie z uzyciem funkcji bibliotecznych
			bSortLib(nameFile, recordAmount,recordSize);
		}
		
		return 0;
	}


//
//	SHUFFLE
//
	
	if(argc==6 && strcmp("shuffle",argv[1])==0){
		nameFile=argv[3];
		recordAmount=atoi(argv[4]);		
		recordSize=atoi(argv[5]);
		if(strcmp("sys",argv[2])==0){
			printf("SYS SHUFFLE : FILE = %s , recordAmount = %d , recordSize = %d \n",nameFile,recordAmount,recordSize);
			//shufflowanie z uzyciem funkcji systemowych		
			ShuffleSys(nameFile, recordAmount,recordSize);
		}else if(strcmp("lib",argv[2])==0){
			printf("LIB SHUFFLE : FILE = %s , recordAmount = %d , recordSize = %d \n",nameFile,recordAmount,recordSize);
			//shufflowanie z uzyciem funkcji bibliotecznych
			ShuffleLib(nameFile, recordAmount,recordSize);
		}
		
		return 0;
	}

	
	printf("Podano niepoprawne argumenty\nPrzyklady : \ngenerate genFile 512 4092 \nsort lib sortFile 128 5000\n");
	
	

return 0;
}
