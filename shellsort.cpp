#include <stdio.h>
#include <cstdlib>
#include "utility.h"
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <stdlib.h>   
#include <sys/times.h>  /* times() */
#include <unistd.h>     /* sysconf() */ 
#include <cstring>

using namespace std;


int main(int argc, char* argv[])
{
	//report time part 1 in sorter
	double t1, t2, cpu_time; struct tms tb1, tb2; double ticspersec;
	ticspersec = (double) sysconf(_SC_CLK_TCK);
	t1 = (double) times(&tb1);

	char* fileName = "";
	int start;
	int end;
	const int numGaps = 8;
	int atr;
	int fd;
 
	// Using Marcin Ciura's gap sequence, with an inner insertion sort.
	// Adopted from https://en.wikipedia.org/wiki/Shellsort
	int gaps[numGaps] = {701, 301, 132, 57, 23, 10, 4, 1};
	

	if(argc != 7){
		printf("Incorrect invocation. Please specify file name, start (of the range), end (of the range), and atribute number \n");
		exit(1);
	}

	fileName = argv[1];
	start = atoi(argv[2]); 
	end = atoi(argv[3]); 
	atr = atoi(argv[4]); 
	fd = atoi(argv[5]);

	pid_t rootPid;
	long int pidL = atoi(argv[6]);
	rootPid = (pid_t)pidL;

	int numberOfRecords = end - start;
	Record* records = readFile(fileName, start, end);

	// Sorting using shellscript
	for (int gap = 0; gap < numGaps; gap++)
	{
	    for (int i = gaps[gap]; i < numberOfRecords; i += 1)
	    {
	        Record temp = records[i];
	        int j;
	        for (j = i; j >= gaps[gap] && atrCompare(atr, records[j - gaps[gap]], temp); j -= gaps[gap])
	        {
	            records[j] = records[j - gaps[gap]];
	        }
	        records[j] = temp;
	    }
	}

	printToPipe(records, numberOfRecords, fd);
	kill(rootPid, SIGUSR1);

	t2 = (double) times(&tb2);
	cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
	reportTime("Shellsort node running", (t2 - t1) / ticspersec, cpu_time / ticspersec);
	
	return 0;
}
