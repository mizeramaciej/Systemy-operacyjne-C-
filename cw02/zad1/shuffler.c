#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "shuffler.h"
#include <sys/types.h>
#include <sys/stat.h>

void ShuffleSys(char *fileName, int recordAmount, int recordSize){
//	printf("ShuffleSys START\n");
	
	srand(time(NULL));

	
	int fp;
	if( (fp=open(fileName,O_RDWR))==-1){
		printf("open error");
		exit(1);
	}
	
	
	char * record1 = malloc(recordSize);
	char * record2 = malloc(recordSize);
	
	int i,j;
	int n=recordAmount;
	   
	
	for( i=n-1; i>=1; i--){
		
		j=rand()%i;
		
		
		lseek(fp, i * recordSize, SEEK_SET);
		read(fp,record1,recordSize); //read(fp,record1,1);
		
		lseek(fp, j * recordSize, SEEK_SET);
		read(fp,record2,recordSize);
		

		lseek(fp, i * recordSize, SEEK_SET);
		if( write(fp,record2,recordSize)!=recordSize){
			printf("write error\n");
			exit(1);
		}
		lseek(fp, j * recordSize, SEEK_SET);
		if( write(fp,record1,recordSize)!=recordSize){
			printf("write error\n");
			exit(1);
		}	
	
	}
		
	
//	printf("ShuffleSys END\n");
    close(fp);
	free(record1);
	free(record2);

	return;
}

void ShuffleLib(char *fileName, int recordAmount, int recordSize){
//	printf("ShuffleLib START\n");
	
	srand(time(NULL));
	
	FILE * fp;
	if( (fp=fopen(fileName,"r+"))==NULL){ //Opens a file to update both reading and writing. The file must exist.
		printf("fopen error\n");
		exit(1);
	}
	
	char * record1 = malloc(recordSize);
	char * record2 = malloc(recordSize);
	int i,j;
	int n=recordAmount;
	
	for( i=n-1; i>=1; i--){
			
		j=rand()%i;

		fseek(fp, i * recordSize, SEEK_SET);

		if( fread(record1,1,recordSize,fp)!=recordSize){
			printf("fread error\n");
			exit(1);
		}

		fseek(fp, j * recordSize, SEEK_SET);
		if( fread(record2,1,recordSize,fp)!=recordSize){
			printf("fread error\n");
			exit(1);
		}



		fseek(fp, i * recordSize, SEEK_SET);
		if( fwrite(record2,1,recordSize,fp)!=recordSize){
			printf("fwrite error\n");
			exit(1);
		}
		fseek(fp, j * recordSize, SEEK_SET);
		if( fwrite(record1,1,recordSize,fp)!=recordSize){
			printf("fwrite error\n");
			exit(1);
		}
		//
		//fsync(fp); //czeka za dane beda zapisane
		//		
			
	}	
		
	
//	printf("ShuffleLib END\n");
    fclose(fp);
	free(record1);
	free(record2);
	return;	
}
