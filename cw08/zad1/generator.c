#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <time.h>


int main(int argc, char* argv[]) {
	if(argc != 4) {
		printf("Example ./generator <file_name> <records_no> <record_length>\n");
		return 0;
	}
	
	char* file_name = argv[1];
	int records_no = atoi(argv[2]);
	int record_length = atoi(argv[3]);
	srand((int)records_no+time(NULL));
	FILE *fp;
	fp = fopen(file_name, "w+");

	int i,j;
	char record[record_length];
	for(i=1; i<=records_no; i++) {
		bzero(record, record_length); //zerowanie stringa
		int id_length = snprintf(record, record_length, "%d.", i);

		record[id_length] = 'A' + (rand() % 26);
		for(j=id_length + 1; j<record_length-1; j++) {
			record[j] = (rand() % 6) ? ('a' + (rand() % 26)) : ' ';
		}
		record[record_length-2] = 'a' + (rand() % 26);
		record[record_length-1] = '\0';
		printf("%s\n", record);
		fprintf(fp,"%s\n", record);
	}

	fclose(fp);
	return 0;
}