#include <stdio.h>
#include <stdlib.h>     /* strtol */
#include "utility.h"
#include <string>
#include <iostream>
#include <cstring>

#define SIZEofBUFF 68
#define SIZEofINT 4
#define SIZEofFIRST 25
#define SIZEofLAST 35
#define SIZEofINCOME 4

void printRecord(Record record){
	printf("Tax Number: %d ", record.taxNum);
	// printf("First Name: %s ", record.firstName);
	std::cout << "First Name: " << record.firstName<<" ";
	std::cout << "Last Name: " << record.lastName << " ";
	// printf("Last Name: %s ", record.lastName);
	printf("Income: %f\n", record.income);

}

char* readFile(char* fileName, int start, int end){
	// Test size
	// 
	FILE *fpb;
	int numOfrecords, i;
	char buffer[SIZEofBUFF];
	numOfrecords = end - start;

	Record records[numOfrecords];

	fpb = fopen (fileName,"rb");
	if (fpb==NULL) {
	  	printf("Cannot open binary file\n");
		exit(1);
	}

	printf("Records found in file %d \n", numOfrecords);

	fseek(fpb, start*SIZEofBUFF, SEEK_SET);
	for (i = 0; i < numOfrecords ; i++) {

		//Figured out the following three lines together with Shantanu Bhatia  
	  	fread(buffer, SIZEofINT, 1, fpb);
	  	int taxNumber;
	  	memcpy(&taxNumber, buffer, SIZEofINT);
	  	printf("Taxation Number: %d\n", taxNumber);

	  	fread(buffer, SIZEofFIRST, 1, fpb);
	  	std::string firstN;
	  	memcpy(&firstN, buffer, SIZEofFIRST);
	  	// printf("%s\n", firstN);
	  	std::cout<<firstN<<std::endl;

	  	fread(buffer, SIZEofLAST, 1, fpb);
	  	std::string lastN;
	  	memcpy(&lastN, buffer, SIZEofLAST);
	  	// printf("%s\n", lastN);
	  	std::cout<<lastN<<std::endl;


	  	fread(buffer, SIZEofINCOME, 1, fpb);
	  	float inc;
	  	memcpy(&inc, buffer, SIZEofINCOME);
	  	printf("fIncome: %f\n\n", inc);

	  	Record record = { .taxNum = taxNumber, .firstName = firstN, .lastName = lastN, .income = inc };
	  	records[i] = record;
	}

	fclose (fpb);

	for(int i = 0; i < numOfrecords; i++){
		printRecord(records[i]);
	}

	char* buf = "str";
	return buf;
}