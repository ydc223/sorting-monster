#include <stdio.h>
#include "utility.h"
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include <stdlib.h>    
#include <cstring>



// void printToPipe(Record* records, int numberOfRecords){
// 	int fd;
//     char * myfifo = "/tmp/myfifo";

//     /* create the FIFO (named pipe) */
//     mkfifo(myfifo, 0666);

//      // write "Hi" to the FIFO 
//     // open with append 
//     fd = open(myfifo, O_WRONLY);
//     // fd = open(myfifo, O_WRONLY|O_NONBLOCK);

//     write(fd, "Hi", sizeof("Hi"));
//     kill(getppid(), SIGUSR1);
    // close(fd);

    /* remove the FIFO */
    // unlink(myfifo);

	// int fd;
	// // FIFO file path
 //    char * myfifo = "/tmp/myfifo";

	// // if( !(access( myfifo, F_OK ) != -1) ) {
	// //     if(mkfifo(myfifo, 0666) == -1) {
	// // 		perror("myfifo");
	// // 		exit(1);
	// // 	}
	// // }
    

	// fd = open(myfifo, O_WRONLY);

 //    // Write the input arr2ing on FIFO
 //    // and close it
 //    // write(fd, records, 68*numberOfRecords);
 //    char start[10];

 //  	sprintf(start, "%d", records[0].taxNum);
 //  	// printf("size of %s is %d\n", start, sizeof(start));
 //    write(fd, start, sizeof(start));
 //    close(fd);
 //    unlink(myfifo);
 //    exit(1);
// }



int main(int argc, char* argv[])
{
	char* fileName = "";
	int start;
	int end;
	const int numGaps = 8;
	int atr;
	int fd;
 
	// Using Marcin Ciura's gap sequence, with an inner insertion sort.
	// Adopted from https://en.wikipedia.org/wiki/Shellsort
	int gaps[numGaps] = {701, 301, 132, 57, 23, 10, 4, 1};
	

	if(argc != 6){
		printf("Incorrect invocation. Please specify file name, start (of the range), end (of the range), and atribute number \n");
		exit(1);
	}

	fileName = argv[1];
	start = atoi(argv[2]); 
	end = atoi(argv[3]); 
	atr = atoi(argv[4]); 
	fd = atoi(argv[5]);


	int numberOfRecords = end - start;
	// printf("%s, %d, %d, %d, %d\n", fileName, start, end, atr, fd);
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


		
	// printf("Pringting sorted records in bubblesort!! YEEEY!\n");
	// for (int i = 0; i < numberOfRecords; i++){
	// 	printRecord(records[i]);
	// }

	printToPipe(records, numberOfRecords, fd);

	return 0;
}
