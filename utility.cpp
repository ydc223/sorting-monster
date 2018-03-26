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
#include <math.h>
#include <iostream>
#include <fstream>
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
	FILE *f = fopen(filename, "a");
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
	char nameBuf[SIZEofFIRST];
	char lastBuf[SIZEofFIRST];

	fseek(fpb, start*SIZEofBUFF, SEEK_SET);
	for (i = 0; i < numOfrecords ; i++) {

		//Figured out the following memcpy method together with Shantanu Bhatia  
	  	fread(buffer, SIZEofINT, 1, fpb);
	  	int taxNumber;
	  	memcpy(&taxNumber, buffer, SIZEofINT);

	  	// Record record = { .taxNum = taxNumber};

	  	
	  	fread(nameBuf, SIZEofFIRST, 1, fpb);
	  	string firstN = nameBuf;
	  	// strcpy(record.firstName, nameBuf);

	  	
	  	fread(lastBuf, SIZEofLAST, 1, fpb);
	  	std::string lastN = lastBuf;
	  	// strcpy(record.lastName, lastBuf);

	  	fread(buffer, SIZEofINCOME, 1, fpb);
	  	float inc;
	  	memcpy(&inc, buffer, SIZEofINCOME);
	  	// record.income = inc;

	  	Record record = { .taxNum = taxNumber, .firstName = firstN, .lastName = lastN, .income = inc };

	  	//printf("%s %s\n", record.firstName, record2.lastName);

	  	records[i] = record;
	}

	fclose (fpb);
	return records;
}

void reportMissingSigs(char* type, int missing, char* outFile){
	if(strcmp(outFile,"none") == 0 ) {
		printf("%d %s signal(s) missing\n", missing, type);
	} else {
		FILE *f = fopen(outFile, "a");
	    if (f == NULL)
	    {
	        printf("Error opening file!\n");
	        exit(1);
	    }
	    // printToPipe(sortedRecords, numOfrecords, f);
	    fprintf(f, "%d %s signal(s) missing\n", missing, type);
	    fclose(f);
	}
}

void checkNumOfSignalsMissing(volatile sig_atomic_t sh_s, volatile sig_atomic_t q_s, volatile sig_atomic_t b_s, int level, char * outFile) {
  int totalNumberOfSignals = pow(2, level);
  int sh_count = 0, q_count = 0, b_count = 0;
  int missing;
  for (int i = 0; i < totalNumberOfSignals; i++) {
    if(i%3 == 0){
      sh_count++;
    } else if (i%3 == 1){
      q_count++;
    } else {
      b_count++;
    }
  }

  if(sh_s != sh_count){
    missing = sh_count - sh_s;
    reportMissingSigs("shellsort", missing, outFile);
    
  }

  if(q_s != q_count){
    missing = q_count - q_s;
    reportMissingSigs("quicksort", missing, outFile);
  }

  if(b_s != b_count){
    missing = b_count - b_s;
    reportMissingSigs("bubblesort", missing, outFile);
  }
}

//This function finds process number based on the range, size and total number of records and calls the appropriate sorting program
void callExec(char* filename, int low, int high, char* atrNumChar, int fd, pid_t rootPid, int totalRecords, int processCount, int rand) {
  char start[5];
  char end[5];
  char fdStr[30];
  char root[30];

  sprintf(fdStr, "%d", fd);
  sprintf(start, "%d", low);
  sprintf(end, "%d", high);
  sprintf(root, "%ld", (long)rootPid);   

  int size = high - low;
  int processNum = totalRecords/size - (totalRecords - low)/size;
  if(rand){
  	processNum = processCount+2;
  }
  // printf("Low: %d high: %d size: %d processNum: %d\n", low, high, size, processNum);
  if(processNum%3 == 0){
    cout<<"shell"<<endl;
    execl("./shellsort", "./shellsort", filename, start, end, atrNumChar, fdStr, root, (char*)NULL);
  } else if (processNum%3 == 1){
    cout<<"quick"<<endl;
    execl("./quicksort", "./quicksort", filename, start, end, atrNumChar, fdStr, root, (char*)NULL);
  } else {
    cout<<"bubble"<<endl;
    execl("./bubblesort", "./bubblesort", filename, start, end, atrNumChar, fdStr, root, (char*)NULL);
  }
}


void reportTime(char * event, float realTime, float cpuTime) {
	FILE *f = fopen("reportedTime.txt", "a");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    fprintf(f, "%s time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n", event, realTime, cpuTime);

    fclose(f);
}

void reportAllTimestamps(char* timeFile, char* outFile){
	string line;
	ifstream myfile (timeFile);
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			if(strcmp(outFile,"none") == 0 ) {
				cout << line << '\n';
			} else {
				ofstream myfile (outFile, std::fstream::app);
				if (myfile.is_open())
				{
					myfile << line<<'\n';
					myfile.close();
				}
				else cout << "Unable to open file";

			    // printToPipe(sortedRecords, numOfrecords, f);
			}		

		}
		myfile.close();
	} else 
		cout << "Unable to open file"; 

}
