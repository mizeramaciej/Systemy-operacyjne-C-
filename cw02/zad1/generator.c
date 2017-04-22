#include <stdio.h>
#include <stdlib.h>
#include "generator.h"

void generateRecordsFile(char *fileName,int recordAmount, int recordSize){
//	printf("generateRecordsFile START \n");
	FILE * fpRandom;
	FILE * fpDest;
	
	if( (fpRandom=fopen("/dev/urandom","r"))==NULL){
		printf("fopen error\n");
		exit(1);
	}
	
	if( (fpDest=fopen(fileName,"w"))==NULL){
		printf("fopen error\n");
		exit(1);
	}
	
	char * record = malloc(recordSize);
	int i;
	for(i=0;i<recordAmount;i++){
		if( fread(record,1,recordSize,fpRandom)!=recordSize){
			printf("fread error\n");
			exit(1);
		}
		
		int j=0;
		for(j=0;j<recordSize;j++){ //zamiana na 'normalne' znaki w 
			record[j]=record[j]%20+65;	//celu latwego sprawdzania sortowania
		}
		
		if( fwrite(record,1,recordSize,fpDest)!=recordSize){
			printf("fwrite error\n");
			exit(1);
		}
	}
	
    fclose(fpRandom);
	fclose(fpDest);
	
	free(record);
//	printf("generateRecordsFile END \n");
}


void copyFile(char *sourceFileName, char *destFileName, int recordAmount, int recordSize) {
//	printf("copyFile START\n");
    FILE *source, *target;

	if( (source=fopen(sourceFileName,"r"))==NULL){
		printf("fopen error\n");
		exit(1);
	}
	if( (target=fopen(destFileName,"w"))==NULL){
		printf("fopen error\n");
		exit(1);
	}
	

    char * record = malloc(recordSize);
	int i;
	for(i=0 ; i<recordAmount ; i++){
		if( fread(record,1,recordSize,source)!=recordSize){
			printf("fread error\n");
			exit(1);
		}	
		
		if( fwrite(record,1,recordSize,target)!=recordSize){
			printf("fwrite error\n");
			exit(1);
		}
	}

    fclose(source);
    fclose(target);

//	printf("copyFile END\n");
}
