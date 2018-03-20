#include <stdio.h>
#include "utility.h"

void bubblesort(){
	printf("buuble\n");
}

int main(int argc, char* argv[])
{
	char* fileName = "";
	int start;
	int end;
	const int numGaps = 2;
	const int n = 11;
	// int gaps[numGaps] = [701, 301, 132, 57, 23, 10, 4, 1];
	int gaps[numGaps] = {4, 1};
	int a[n] = {0, 5, 9, 1, 15, 2, 7, 3, 8, 90, 4};

	if(argc != 4){
		printf("No file descriptior or range specified\n");
		exit(1);
	}

	fileName = argv[1];
	start = atoi(argv[2]); 
	end = atoi(argv[3]); 
	printf("%s, %d, %d\n", fileName, start, end);
	readFile(fileName, start, end);


	// Sorting using shellscript
	for (int gap = 0; gap < numGaps; gap++)
	{
	    for (int i = gaps[gap]; i < n; i += 1)
	    {
	        int temp = a[i];
	        int j;
	        for (j = i; j >= gaps[gap] && a[j - gaps[gap]] > temp; j -= gaps[gap])
	        {
	            a[j] = a[j - gaps[gap]];
	        }
	        a[j] = temp;
	    }
	}

	for (int i = 0; i < n; i++){
		printf("%d " , a[i]);
	}
	printf("\n");
	

	return 0;
}

