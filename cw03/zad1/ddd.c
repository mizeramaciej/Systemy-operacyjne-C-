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

   return 0;
}