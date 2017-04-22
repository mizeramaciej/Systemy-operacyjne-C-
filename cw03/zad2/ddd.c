#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
	
    if (argc != 2) {
        printf("You need to give me envVariable as an argument.\n");
        return 0;
    }

    const char *env = getenv(argv[1]);
	
    if (env == NULL) {
        printf("Couldn't find envVariable %s\n", argv[1]);
    } else {
        printf("envVariable %s = %s\n",argv[1], env);
    } 
	
	int i=1;		
	char* t;
	while(i<100){
		t=calloc(i,10e5);
		if(t==NULL){
			printf("out of memory i=%d\n",i);
			return 1;
		}
		int j;
		for(j=0;j<i*10e5;j++){
			t[j]=3;	
		}
		
		printf("%d,",i);
		i++;
	}
	
	free(t);
   return 0;
}