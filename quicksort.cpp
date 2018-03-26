#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <sys/times.h>  /* times() */
#include <unistd.h>     /* sysconf() */
#include "utility.h"

/* takes the last element as the pivot and sorts accordingly */
int sortPartition (int atr, Record *records, int low, int high)
{
    Record pivot = records[high];    // pivot
    int i = (low - 1);  // Index of smaller element
 
    for (int j = low; j <= high - 1; j++)
    {
        // If pivot is greater than the current element
        if (atrCompare(atr, pivot, records[j]))
        {
            i++;    // increment index of smaller element
            swap(&records[i], &records[j]);
        }
    }
    swap(&records[i + 1], &records[high]);
    return (i + 1);
}
 
/* sorts using sortPartition based on the pivot and then performs sorts on either side of the partition */
// Recursion idea is adopted from https://www.geeksforgeeks.org/quick-sort/
void quickSort(int atr, Record* records, int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, records[p] is now at right place */
        int pi = sortPartition(atr, records, low, high);
 
        /* Separately sort elements before partition and after partition */
        quickSort(atr, records, low, pi - 1);
        quickSort(atr, records, pi + 1, high);
    }
}

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
	Record *records = readFile(fileName, start, end);

	quickSort(atr, records, 0, numberOfRecords-1);

	printToPipe(records, numberOfRecords, fd);
	kill(rootPid, SIGUSR2);

	t2 = (double) times(&tb2);
	cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
	reportTime("Quicksort node running", (t2 - t1) / ticspersec, cpu_time / ticspersec);
	
	return 0;
}
