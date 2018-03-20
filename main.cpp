#include <stdio.h> /* printf() */
#include <sys/times.h>  /* times() */
#include <unistd.h>     /* sysconf() */
#include <string.h>   /* strncmp() */
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include "utility.h"

using namespace std;

#define SIZEofBUFF 68
#define MAX_BUF 1024
#define SIZEofINT 4
#define SIZEofINCOME 4


volatile sig_atomic_t sh_s = 0;
volatile sig_atomic_t q_s = 0;


void handle_sigusr1(int signum) {
  cout<<"helloUSR1"<<endl;
  sh_s = 1;
}


void handle_sigusr2(int signum) {
  cout<<"helloUSR2"<<endl;
  q_s = 1;
}

Record * merge(Record * records1, int size1, Record * records2, int size2, char* atrNumChar){
  int size = size1 + size2;
  Record * records = new Record[size];

  int atrNum = atoi(atrNumChar);
  // printf("-----------MERGEEEEE-----------\n");

  int i = 0, j = 0, k = 0;
  //if first one is greater then second
  while (i < size1 && j < size2) {
    if (atrCompare(atrNum, records1[i], records2[j])){
      records[k] = records2[j];
      j++;
    } else {
      records[k] = records1[i];
      i++;
    }
    k++;
  }

  /* Copy the remaining elements of L[], if there
       are any */
  while (i < size1) {
    records[k] = records1[i];
    i++;
    k++;
  }
 
  /* Copy the remaining elements of R[], if there
     are any */
  while (j < size2)
  {
    records[k] = records2[j];
    j++;
    k++;
  } 

  return records;
}

Record* split(char * filename, char * atrNumChar, int depth, int low, int numOfrecords) {

  char start[5];
  char end[5];
  char fdStr1[30];
  char fdStr2[30];
  
  pid_t cpid;
  pid_t cpid2;
  char buff[20];
  int fd1[2];  // Used to store two ends of first pipe
  int fd2[2];  // Used to store two ends of second pipe
  // printf("low: %d numofRecord: %d, depth: %d\n", low, numOfrecords, depth );
  Record * recordsMerged;

  if (depth - 1 != 0) {
    //fork before proceeding
    // explore the bugs and fix
    // have fun 
    if (pipe(fd1) == -1) {
      fprintf(stderr, "Pipe Failed" );
      exit(1);
    }
    if (pipe(fd2) == -1) {
      fprintf(stderr, "Pipe Failed" );
      exit(1);
    }

    if((cpid = fork()) == 0) {
      Record* recordsMerged1 = split(filename, atrNumChar, depth - 1, low, numOfrecords/2+low/2);
      close(fd1[0]);
      printToPipe(recordsMerged1, numOfrecords - (numOfrecords/2+low/2), fd1[1]);
      exit(0);
    } else {
      //Parent
      if((cpid2 = fork()) == 0) {
        //Child 2
        Record* recordsMerged2 = split(filename, atrNumChar, depth - 1, numOfrecords/2+low/2, numOfrecords); 
        close(fd2[0]);
        printToPipe(recordsMerged2, numOfrecords - (numOfrecords/2+low/2), fd2[1]);
        exit(0);
      } else {
        close(fd1[1]); 
        close(fd2[1]);

        Record* records1 = readFromPipe(fd1, numOfrecords - (numOfrecords/2+low/2));
        // printf("Pringting sorted records in main!! YEEEY!\n");
        // printRecords(records1, numOfrecords/2);
        
        Record* records2 = readFromPipe(fd2, numOfrecords - (numOfrecords/2+low/2));
        // printf("Pringting sorted records in main!! YEEEY!\n");
        // printRecords(records2, numOfrecords/2);

        waitpid(cpid, NULL, 0);
        waitpid(cpid2, NULL, 0);
        // wait(NULL);
        // wait(NULL);
        // printf("SHOULD E WRITTEN ONLY ONCE IF LEVEL IS 2!\n");


        recordsMerged = merge(records1, numOfrecords - (numOfrecords/2+low/2), records2, numOfrecords - (numOfrecords/2+low/2), atrNumChar);
        // recordsMerged = records1;
        // printRecords(recordsMerged, 2 * (numOfrecords - (numOfrecords/2+low/2)));
      }
    } 

  } else {
    if (pipe(fd1) == -1) {
      fprintf(stderr, "Pipe Failed" );
      exit(1);
    }
    if (pipe(fd2) == -1) {
      fprintf(stderr, "Pipe Failed" );
      exit(1);
    }

    if((cpid = fork()) == 0) {
      //child 1
      close(fd1[0]);
      sprintf(fdStr1, "%d", fd1[1]);
      sprintf(start, "%d", low);
      sprintf(end, "%d", numOfrecords/2+low/2);
      // printf("First start: %s, end: %s\n", start, end);
      execl("./shellsort", "./shellsort", filename, start, end, atrNumChar, fdStr1,  (char*)NULL);
      perror("exec failure.. ");
      exit(1);
    } else {
      //parent process 
      if((cpid2 = fork()) == 0){
        //child 2 
        close(fd2[0]);
        sprintf(fdStr2, "%d", fd2[1]);
        sprintf(start, "%d", numOfrecords/2+low/2);
        sprintf(end, "%d", numOfrecords);
        // printf("Second start: %s, end: %s\n", start, end);
        execl("./quicksort", "./quicksort", filename, start, end, atrNumChar, fdStr2, (char*)NULL);
        perror("exec failure.. ");
        exit(1);
      } else {
        // Parent
        close(fd1[1]); 
        close(fd2[1]);

        // printf("Num of Records: %d, low: %d\n", numOfrecords, low);
        int size = numOfrecords - (numOfrecords/2+low/2);

        // printf("Upper range bound: %d\n", numOfrecords);
        Record* records1 = readFromPipe(fd1, size);
        // printf("Pringting %d sorted records in main!! YEEEY!\n", size);
        // printRecords(records1, size);
        
        Record* records2 = readFromPipe(fd2, size);

        // printf("Pringting %d sorted records in main!! YEEEY!\n", size);
        // printRecords(records2, size);

        waitpid(cpid, NULL, 0);
        waitpid(cpid2, NULL, 0);
        wait(NULL);
        wait(NULL);


        recordsMerged = merge(records1, size, records2, size, atrNumChar);
        // printRecords(recordsMerged, numOfrecords);
        close(fd1[0]);
        close(fd2[0]);
      }
    }
    wait(NULL);

  }
  return recordsMerged;
}

int main(int argc, char* argv[])
{
  //report time part 1
  double t1, t2, cpu_time; struct tms tb1, tb2; double ticspersec;
  ticspersec = (double) sysconf(_SC_CLK_TCK);
  t1 = (double) times(&tb1);

  int i = 1;
  int depth = 1;
  char * filename = "";
  int atrNum = 0;
  char* atrNumChar;
  char *outFile = "outputFile.txt";
  int random = 0;
  int numOfrecords;
  long lSize;
  FILE *fpb;
  int status;
  pid_t pid;
  Record * sortedRecords;

  // char * myfifo = "/tmp/myfifo";
 
    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
  // mkfifo(myfifo, 0666);
  while(i < argc){
    char *flag = (argv[i]);
    if(strncmp (flag,"-d", 2) == 0){
      depth = std::atoi(argv[i+1]);
      printf("Depth level: %d\n", depth);
      i+=2;
    } else if(strncmp (flag,"-f", 2) == 0){
      filename = argv[i+1];
      printf("Filename: %s\n", filename);
      i+=2;
    } else if(strncmp (flag,"-a", 2) == 0){
      atrNum = std::atoi(argv[i+1]);
      atrNumChar = argv[i+1];
      if (atrNum > 3 || atrNum < 0){
        printf("Please input valid attribute number\n");
        return 0;
      }
      printf("Atribute Number %d\n", atrNum);
      i+=2;
    } else if(strncmp (flag,"-o", 2) == 0){
      outFile = argv[i+1];
      printf("Output file: %s\n", outFile);
      i+=2;
    } else if(strncmp (flag,"-r", 2) == 0){
      random = 1;
      printf("r\n");
      i++;
    }
  }

  fpb = fopen (filename,"rb");
  if (fpb==NULL) {
      printf("Cannot open binary file\n");
    exit(1);
  }

  //finding the number of records
  fseek (fpb , 0 , SEEK_END);
  lSize = ftell (fpb);
  rewind (fpb);
  numOfrecords = (int) lSize/ SIZEofBUFF;
  printf("Records found in file %d \n", numOfrecords);
  numOfrecords = 160;
  
  signal(SIGUSR1, handle_sigusr1);
  signal(SIGUSR2, handle_sigusr2); 


  // if (pipe(fd1) == -1) {
  //   fprintf(stderr, "Pipe Failed" );
  //   exit(1);
  // }

  //root has one child
  if((pid = fork()) == 0) {
    //child process
    sortedRecords = split(filename, atrNumChar, depth, 0, numOfrecords);
    printf("\n\n" );
    printRecords(sortedRecords, numOfrecords);
    printToFile(outFile, sortedRecords, numOfrecords);
    
    exit(0);
  } else{
    waitpid(pid, NULL, 0);

  }

    //report time part 2
  // printf("%d\n", merge);
  t2 = (double) times(&tb2);
  cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
  printf("Run time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n", (t2 - t1) / ticspersec, cpu_time / ticspersec);
  return 0;
  
}

