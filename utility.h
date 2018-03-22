#ifndef SORT_H   /* Include guard */
#define SORT_H
#include <string>

struct Record* readFile(char* fd, int start, int end);
void printRecords(Record * record, int numOfrecords);
int atrCompare(int, Record, Record);
void swap(Record* a, Record* b);
void printToPipe(Record* records, int numberOfRecords, int fd);
Record * readFromPipe(int * fd, int numOfrecords);
void printToFile(char* filename, Record* sortedRecords, int numOfrecords);
void checkNumOfSignalsMissing(volatile sig_atomic_t sh_s, volatile sig_atomic_t q_s, volatile sig_atomic_t b_s, int level, char*);
void callExec(char* filename, int low, int high, char* atrNumChar, int fd, pid_t rootPid, int totalRecords);
void reportTime(char * event, float realTime, float cpuTime);
void reportAllTimestamps(char* timeFile, char* outFile);

typedef struct Record {
	int taxNum;
	// char firstName[25];
	// char lastName[35];
	std::string firstName;
	std::string lastName;
	float income;
} Record;

#endif // FOO_H_
