#include <stdio.h>
#include <stdlib.h>    
#include <string>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "utility.h"


#define SIZEofBUFF 68
#define SIZEofINT 4
#define SIZEofFIRST 25
#define SIZEofLAST 35
#define SIZEofINCOME 4

using namespace std;

Record * readFromPipe(int * fd, int numOfrecords) {
  Record * records = new Record[numOfrecords];
  char str1[SIZEofBUFF];
  char str2[SIZEofBUFF];

  for(int i = 0; i < numOfrecords; i++) {
    read(fd[0], str1, 4);
    int taxNumber;
    memcpy(&taxNumber, str1, SIZEofINT);

    read(fd[0], str1, 25);
    string firstN = str1; 

    read(fd[0], str1, 35);
    string lastN = str1; 

    read(fd[0], str1, 4);
    float inc;
    memcpy(&inc, str1, SIZEofINCOME);
    // printf("Recieved: %d of ", taxNumber);
    // cout << firstN << " " << lastN << " "; 
    // printf("with income %f\n", inc);

    Record record = { .taxNum = taxNumber, .firstName = firstN, .lastName = lastN, .income = inc };
    records[i] = record;
  }
  return records;
}

void printToPipe(Record* records, int numberOfRecords, int fd){
    char taxNum[4];
    char income[4];
    char firstN[25];
    char lastN[35];

    // Passing information to to the parent process
	for (int i = 0; i < numberOfRecords; i++){
		memcpy(taxNum, &records[i].taxNum, 4);
	    memcpy(income, &records[i].income, 4);
	    strncpy(firstN, records[i].firstName.c_str(), sizeof(firstN));
	    strncpy(lastN, records[i].lastName.c_str(), sizeof(lastN));

	  	// printf("taxNum %s income %s first name %s last name %s\n", taxNum, income, firstN, lastN);
	  	write(fd, taxNum, sizeof(taxNum));
		write(fd, firstN, sizeof(firstN));
		write(fd, lastN, sizeof(lastN));
		write(fd, income, sizeof(income));
	}

	// maybe comment out?
    close(fd);
    // kill(getppid(), SIGUSR1);
}

void printToFile(char* filename, Record* sortedRecords, int numOfrecords) {
	FILE *f = fopen(filename, "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    // printToPipe(sortedRecords, numOfrecords, f);
    for(int i = 0; i < numOfrecords; i++) {
      fprintf(f, "Tax Number: %d, Name: %s %s, Income: %f\n", sortedRecords[i].taxNum, sortedRecords[i].firstName.c_str(), sortedRecords[i].lastName.c_str(), sortedRecords[i].income);
    }
    fclose(f);
}

// Function for swaping to elements
void swap(Record* a, Record* b)
{
    Record t = *a;
    *a = *b;
    *b = t;
}

// Function for printing records
void printRecords(Record * record, int numOfrecords){
	for (int i = 0; i < numOfrecords; i++){
		printf("Tax Number: %d ", record[i].taxNum);
		// printf("First Name: %s ", record.firstName);
		// printf("Last Name: %s ", record.lastName);
		std::cout << "First Name: " << record[i].firstName<<" ";
		std::cout << "Last Name: " << record[i].lastName << " ";
		printf("Income: %f\n", record[i].income);
	}
}

// Function for comparing atributes used across all the sorting algorithms
// It decides on which attribute to compare based on the atribute number 
// Idea for implementing this function was developed with Shantanu Bhatia (just an idea, none of the code was shared)
int atrCompare(int atr, Record record, Record tempRecord){

	switch(atr){
		case 0: 
			return(record.taxNum > tempRecord.taxNum);
		case 1: 
			return(record.firstName > tempRecord.firstName);
		case 2: 
			return(record.lastName > tempRecord.lastName);
		case 3: 
			return(record.income > tempRecord.income);
		default:
			printf("Invalid attribute number\n");
			exit(1);
	}
}

// Reads the specified range of the binary file and return an array of records
Record* readFile(char* fileName, int start, int end){
	// Test size
	// 
	FILE *fpb;
	int numOfrecords, i;
	char buffer[SIZEofBUFF];
	numOfrecords = end - start;

	Record * records = new Record[numOfrecords];

	fpb = fopen (fileName,"rb");
	if (fpb==NULL) {
	  	printf("Cannot open binary file\n");
		exit(1);
	}

	// printf("Records found in file %d \n", numOfrecords);

	fseek(fpb, start*SIZEofBUFF, SEEK_SET);
	for (i = 0; i < numOfrecords ; i++) {

		//Figured out the following memcpy method together with Shantanu Bhatia  
	  	fread(buffer, SIZEofINT, 1, fpb);
	  	int taxNumber;
	  	memcpy(&taxNumber, buffer, SIZEofINT);

	  	// Record record = { .taxNum = taxNumber};

	  	char nameBuf[SIZEofFIRST];
	  	fread(nameBuf, SIZEofFIRST, 1, fpb);
	  	string firstN = nameBuf;
	  	// strcpy(record.firstName, nameBuf);

	  	char lastBuf[SIZEofFIRST];
	  	fread(lastBuf, SIZEofLAST, 1, fpb);
	  	std::string lastN = lastBuf;
	  	// strcpy(record.lastName, lastBuf);

	  	fread(buffer, SIZEofINCOME, 1, fpb);
	  	float inc;
	  	memcpy(&inc, buffer, SIZEofINCOME);
	  	// record.income = inc;

	  	Record record = { .taxNum = taxNumber, .firstName = firstN, .lastName = lastN, .income = inc };

	  	// printf("%s %s\n", record2.firstName, record2.lastName);

	  	records[i] = record;
	}

	fclose (fpb);
	return records;
}


// void printToPipe(Record* records, int numberOfRecords){
// 	int fd;
//     char * myfifo = "/tmp/myfifo";

//     /* create the FIFO (named pipe) */
//     mkfifo(myfifo, 0666);

//      // write "Hi" to the FIFO 
//     fd = open(myfifo, O_WRONLY);
//     write(fd, "Hi", sizeof("Hi"));
//     kill(getppid(), SIGUSR1);
//     close(fd);

//     /* remove the FIFO */
//     unlink(myfifo);

// 	// int fd;
// 	// // FIFO file path
//  //    char * myfifo = "/tmp/myfifo";

// 	// // if( !(access( myfifo, F_OK ) != -1) ) {
// 	// //     if(mkfifo(myfifo, 0666) == -1) {
// 	// // 		perror("myfifo");
// 	// // 		exit(1);
// 	// // 	}
// 	// // }
    

// 	// fd = open(myfifo, O_WRONLY);

//  //    // Write the input arr2ing on FIFO
//  //    // and close it
//  //    // write(fd, records, 68*numberOfRecords);
//  //    char start[10];

//  //  	sprintf(start, "%d", records[0].taxNum);
//  //  	// printf("size of %s is %d\n", start, sizeof(start));
//  //    write(fd, start, sizeof(start));
//  //    close(fd);
//  //    unlink(myfifo);
//  //    exit(1);
// }