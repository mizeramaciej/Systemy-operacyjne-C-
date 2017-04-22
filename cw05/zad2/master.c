#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int **T;
int R;
int childrenNo=7;

const double rMin=-2;
const double rMax=1;
const double iMin=-1;
const double iMax=1;


int scale(double value,int min,int max){
	int res=(int) ((value-min)/(double)(max-min)*(double)R);
	return res;
}

void saveT(int **T){
	FILE *data=fopen("data","w+");
	if(data==NULL){
		printf("fopen erron\n");
		exit(1);
	}
	int i,j;
	for(i=0;i<R;i++){
		for(j=0;j<R;j++){
			fprintf(data,"%d %d %d\n",i,j,T[i][j]);	
		}
	}
	fclose(data);
}

void readValues(int **T,char* path){
	
	int fp=open(path,O_RDONLY);
	if(fp<0){
		printf("open error");
		exit(1);
	}
	sleep(3);
	
	int re,im,iter;
	char buf[50];
	while(read(fp,buf,50)>0){

		char* cre=strtok(buf," \n\t\r");
		char* cim=strtok(NULL," \n\t\r");
		char* citer=strtok(NULL," \n\t\r");
	
		re=scale(atof(cre),rMin,rMax);
		im=scale(atof(cim),iMin,iMax);		
		sscanf(citer,"%d",&iter);
		
		T[re][im]=iter;
	}
	
	close(fp);
}

void draw(){
	FILE *plot=popen("gnuplot","w");
	if(plot==NULL){
		printf("gnuplot error\n");
		exit(1);
	}
	fprintf(plot,"set view map\n");
	fprintf(plot,"set xrange [0:%d]\n",R);
	fprintf(plot,"set yrange [0:%d]\n",R);
	fprintf(plot,"plot 'data' with image\n");
	
	fflush(plot);
	getc(stdin);
	pclose(plot);
}

int main(int argc, char *argv[]){
	
	
	if(argc!=3){
		printf("2 Args needed; <path> <R> \n");
		return 1;
	}
	
	int i,j;
	char* path=calloc(strlen(argv[1]),sizeof(char));
	strcpy(path,argv[1]);
	R=atoi(argv[2]);
	printf("\nMASTER START path %s, R %d\n",path,R);
	T=malloc(R*sizeof(int*));
	for(i=0;i<R;i++){
		T[i]=malloc(R*sizeof(int));	
	}
	for(i=0;i<R;i++){
		for(j=0;j<R;j++){
			T[i][j]=0;	
		}
	}
	
	if(mkfifo(path,0666) == -1){ //rw-rw-rw-
        printf("mkfifo error\n");      
        exit(1);
    }
	
	for(i=0; i<childrenNo;i++){
		char* ex[5]={"slave",path,"500000","150",NULL};
		int pid =fork();
		if(pid==0){
			if(execv(ex[0],ex)==-1){
				printf("exec error\n");	
			}
			exit(0);
		}
	}
	
	readValues(T,path);
	saveT(T);
	draw();
	
	printf("MASTER END\n");
	free(path);	
	for(i=0;i<R;i++)
		free(T[i]);
	free(T);
	return 0;
}




