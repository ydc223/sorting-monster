#include <stdio.h>

void shellsort(){
	printf("shellsort\n");
}


int main(int argc, char* argv[])
{
	char* fileDesc = "";
	int start;
	int end;


	if(argc == 4){
		fileDesc = argv[1];
		start = atoi(argv[2]);
		end = atoi(argv[3]);
		printf("%s, %d, %d\n", fileDesc, start, end);
	} else {
		printf("No file descriptior or range specified\n");
	}

	return 0;
}