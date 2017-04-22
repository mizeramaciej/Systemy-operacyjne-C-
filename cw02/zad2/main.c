#define _XOPEN_SOURCE 500
#include <stdint.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <time.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

int byteSize=999;

void checkReg(char * fullpath,int sizeRest){
	struct stat fileStat;
	
	if(lstat(fullpath,&fileStat) < 0) {
		printf("error lstat");
	}
	
	if(S_ISREG(fileStat.st_mode)!=1){
		return;	
	}
	
	if(fileStat.st_size<=sizeRest){
		printf("\n%s",fullpath);
		
		printf("\n\t%u bytes\n\t", (unsigned int)fileStat.st_size);

		printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
		printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
		printf( (fileStat.st_mode & S_IROTH) ? "r" : "-"); 
		printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");

		char* t=ctime(&(fileStat.st_mtime));
		t[strlen(t)-1]='\0';
		printf("\n\t%s\n", t);

	}
	
}

void search(char* root_path, char * child,  int bytes){
		
    if(child)
    {
    //    if (strcmp(child, ".") == 0) return;
    //    if (strcmp(child, "..") == 0) return;
        strcat(root_path, child);
        strcat(root_path, "/");
    }
	
	
    DIR* root=opendir(root_path);
	char *copy=malloc(strlen(root_path) *sizeof(char));
	strcpy(copy,root_path);
	char *tmpRoot=calloc(1024,1);
	
	struct dirent* dent;
    dent=readdir(root);
		
	
    while(dent){
		
		strcpy(tmpRoot,root_path);
		char * dentPath;
		dentPath=calloc(1024,1);
		strcat(dentPath,tmpRoot);
		strcat(dentPath,dent->d_name);
	
		
		if(strcmp(dent->d_name,".")==0 || strcmp(dent->d_name,"..")==0 ){
			dent=readdir(root);
			continue;
		}
		
		//INNY sposob sprawdzania czy jest DIR czy REG
		/*			
			if(dent->d_type == DT_DIR){
				search(tmpRoot,dent->d_name, bytes);
			}else if(dent->d_type == DT_REG){
				checkReg(dentPath,bytes);	
			}
		*/	
		
			
		struct stat statbuf;
		if(lstat(dentPath,&statbuf) < 0) {
			printf("error lstat");
		}
		if(S_ISDIR(statbuf.st_mode)==1){
			search(tmpRoot,dent->d_name, bytes);
		}	
		if(S_ISREG(statbuf.st_mode)==1){
			checkReg(dentPath,bytes);
		}
			
		
        dent=readdir(root);

    }

    closedir(root);

}

char* validatePath(char* path){
	
	char*validPath=path;
	size_t length = strlen(path);
	
	if(path[0]!='/'){ //gdy sciezka jest wzgledna
		char buf[PATH_MAX+1];
		char * realPath=realpath(path,buf);
		path=realPath;
	}

	length = strlen(path);
    if (path[length - 1] != '/') { //gdy nie konczy sie na /
        validPath = calloc(length+2, sizeof(char));//malloc((length + 2) * sizeof(char));
        strcpy(validPath, path);
        validPath[length] = '/';
        validPath[length + 1] = '\0'; //powinna sie konczyc znakiem pustym
   
    }
	return validPath;
}

static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
	
	char *path=(char*)fpath;
	int sizeRest=byteSize;
	checkReg(path,sizeRest);
	
	return 0;	
}

void searchnftw(char *fullPath,int sizeRestriction){
	
	byteSize=sizeRestriction;
	int flags=0;
	flags |= FTW_DEPTH; // |= alternatywa i przypisanie
	flags |= FTW_PHYS; //dzieki tej opcji nftw nie podaza za dowiazaniami symbolicznymi
	printf("START %s\n",fullPath);
	
	if( nftw(fullPath,display_info,20,flags) ==-1){
		printf("error nftw");
		exit(EXIT_FAILURE);
	}
	
	return;
}

int main(int argc, char* argv[]){

	if(argc!=4){
		printf("Musisz podać 3 argumenty np: ./main /home/.../ 10000 normal\n./main /home/.../ 10000 unftw");
		return 1;
	}
	char *startPath;
	int sizeRestriction;
	startPath=argv[1];	
	sizeRestriction=atoi(argv[2]);
	
	startPath=validatePath(startPath);
	
	if(strcmp(argv[3],"normal")==0 ){
		search(startPath,NULL,sizeRestriction);
	}
	else if(strcmp(argv[3],"unftw")==0 ){
		searchnftw(startPath,sizeRestriction);
	}
	
	
	return 0;
}

