#include <stdio.h>
#include "utility.h"

int main(int argc, char* argv[])
{
	char* fileName = "";
	int start;
	int end;
	int atr;

	if(argc != 5){
		printf("Incorrect invocation. Please specify file name, start (of the range), end (of the range), and atribute number \n");
		exit(1);
	}

	fileName = argv[1];
	start = atoi(argv[2]); 
	end = atoi(argv[3]); 
	atr = atoi(argv[4]); 

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
		
	printf("Pringting sorted records in bubblesort!! YEEEY!\n");
	printRecords(records, numberOfRecords);

	return 0;
}