#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <complex.h>
#include <math.h>

const double rMin=-2;
const double rMax=1;
const double iMin=-1;
const double iMax=1;


int iterRand(double re,double im,int K){

	double nre=0,tnre=0;
	double nim=0;
	int i=0;
	for(i=0;i<K;i++){
		tnre=nre;
		nre= nre*nre - nim*nim + re;
		nim= 2*tnre*nim + im;
		
		if( sqrt(nre*nre + nim*nim)>=2)
			break;
	}
	return i;
}

int main(int argc, char *argv[]){

	
	srand((unsigned int)getpid());
	if(argc!=4){
		printf("3 Args needed; <path> <N> <K>\n");
		return 1;
	}
	
	char* path=argv[1];
	int N=atoi(argv[2]);
	int K=atoi(argv[3]);
		
	int fp=open(path,O_WRONLY);	
	if(fp<0){
		fprintf(stderr,"fopen error\n");
		exit(1);
	}
	
	char buf[50];
	int i;
	for(i=0;i<N;i++){
		double re= ( (double)rand() / (double)RAND_MAX) * (rMax-rMin) + rMin;
		double im= ( (double)rand() / (double)RAND_MAX) * (iMax-iMin) + iMin;
		int iter=iterRand(re,im,K);

		sprintf(buf,"%lf %lf %d\n",re,im,iter);
		write(fp,buf,50);
	
	}	
	
	close(fp);
	return 0;
}



