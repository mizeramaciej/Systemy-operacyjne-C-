#ifndef _GENERATOR_H_
#define _GENERATOR_H_

void generateRecordsFile(char *fileName,int recordAmount, int recordSize);

void copyFile(char *sourceFileName, char *destFileName, int lineLength, int numOfRecords);

#endif
