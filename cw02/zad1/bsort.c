#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "bsort.h"

void bSortSys(char *fileName, int recordAmount, int recordSize){
//	printf("bSortSys START\n");
	int fp;
	if( (fp=open(fileName,O_RDWR))==-1){
		printf("open error");
		exit(1);
	}
	
	char * record1 = malloc(recordSize);
	char * record2 = malloc(recordSize);
	int i,elemNumber=0,change=0;
	int n=recordAmount;
	   
	do{
		elemNumber=0;
		change=0;
		for( i=0; i<n-1; i++){
			lseek(fp, elemNumber * recordSize, SEEK_SET);
	
			if( read(fp,record1,recordSize)!=recordSize){
				printf("read error\n");
				exit(1);
			}
			if( read(fp,record2,recordSize)!=recordSize){
				printf("read error\n");
				exit(1);
			} 
			
			if(record1[0]>record2[0]){
				lseek(fp, elemNumber * recordSize, SEEK_SET);
				if( write(fp,record2,recordSize)!=recordSize){
					printf("write error\n");
					exit(1);
				}
				if( write(fp,record1,recordSize)!=recordSize){
					printf("write error\n");
					exit(1);
				}
				change=1;
			}
			
			elemNumber++;
		}
		n=n-1;		
		lseek(fp, 0, SEEK_SET);	
		
		if(change==0) break;
		
	}while(n>1);
		
		
	
//	printf("bSortSys END\n");
    close(fp);
	free(record1);
	free(record2);  
	return;
}

void bSortLib(char *fileName, int recordAmount, int recordSize){ //te z f
	
//	printf("bSortLib START\n");
	
	FILE * fp;
	if( (fp=fopen(fileName,"r+"))==NULL){ //Opens a file to update both reading and writing. The file must exist.
		printf("fopen error\n");
		exit(1);
	}
	
	char * record1 = malloc(recordSize);
	char * record2 = malloc(recordSize);
	int i,elemNumber=0,change=0;
	int n=recordAmount;
	do{
		elemNumber=0;
		change=0;
		for( i=0; i<n-1; i++){
			
			fseek(fp, elemNumber * recordSize, SEEK_SET);
			
			if( fread(record1,1,recordSize,fp)!=recordSize){
				printf("fread error\n");
				exit(1);
			}
			if( fread(record2,1,recordSize,fp)!=recordSize){
				printf("fread error\n");
				exit(1);
			}
			
			if(record1[0]>record2[0]){
				
				fseek(fp, elemNumber * recordSize, SEEK_SET);
				if( fwrite(record2,1,recordSize,fp)!=recordSize){
					printf("fwrite error\n");
					exit(1);
				}
				if( fwrite(record1,1,recordSize,fp)!=recordSize){
					printf("fwrite error\n");
					exit(1);
				}
				//
				//fsync(fp); //czeka za dane beda zapisane
				//
				change=1;
			}
			 	
			elemNumber++;
		}	
		n=n-1;		
		fseek(fp, 0, SEEK_SET);	
		
		if(change==0) break;
		
	}while(n>1);
				
	
//	printf("bSortLib END\n");
    fclose(fp);
	free(record1);
	free(record2);
	return;	
}
