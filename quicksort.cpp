#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
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
	char* fileName = "";
	int start;
	int end;
	const int numGaps = 8;
	int atr;
	int fd;


	if(argc != 6){
		printf("Incorrect invocation. Please specify file name, start (of the range), end (of the range), and atribute number \n");
		exit(1);
	}

	fileName = argv[1];
	start = atoi(argv[2]); 
	end = atoi(argv[3]); 
	atr = atoi(argv[4]); 
	fd = atoi(argv[5]); 
	// sscanf(argv[5], "%p", (int **)&fd);


	int numberOfRecords = end - start;
	// printf("%s, %d, %d, %d, %d\n", fileName, start, end, atr, fd);
	Record *records = readFile(fileName, start, end);

	quickSort(atr, records, 0, numberOfRecords-1);

		
	// printf("Pringting sorted records in quicksort!! YEEEY!\n");
	// for (int i = 0; i < numberOfRecords; i++){
	// 	printRecord(records[i]);
	// }
	printToPipe(records, numberOfRecords, fd);


	return 0;
}
