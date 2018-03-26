#include <stdio.h>
#include <signal.h>
#include <cstdlib>
#include <sys/times.h>  /* times() */
#include <unistd.h>     /* sysconf() */
#include "utility.h"

int main(int argc, char* argv[])
{

	//report time part 1 in sorter
	double t1, t2, cpu_time; struct tms tb1, tb2; double ticspersec;
	ticspersec = (double) sysconf(_SC_CLK_TCK);
	t1 = (double) times(&tb1);


	char* fileName = "";
	int start;
	int end;
	int atr;
	int fd;


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


	// Sorting using bubblesort
	for (int i = 0; i < numberOfRecords-1; i++){      
	   for (int j = 0; j < numberOfRecords-i-1; j++){
	       if (atrCompare(atr, records[j], records[j+1])){
	       		swap(&records[j], &records[j+1]);
	       }
	   }
	}
	
   	printToPipe(records, numberOfRecords, fd);
	kill(rootPid, SIGCONT);

	t2 = (double) times(&tb2);
	cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
	reportTime("Bubblesort node running", (t2 - t1) / ticspersec, cpu_time / ticspersec);
	return 0;
}