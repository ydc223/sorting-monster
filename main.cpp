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
#include <sys/wait.h>
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
volatile sig_atomic_t b_s = 0;


void handle_sigusr1(int signum) {
  cout<<"helloUSR1"<<endl;
  sh_s++;
}

void handle_sigusr2(int signum) {
  cout<<"helloUSR2"<<endl;
  q_s++;
}

void handle_kill(int signum) {
  cout<<"helloKILL"<<endl;
  b_s++;
}

Record * merge(Record * records1, int size1, Record * records2, int size2, char* atrNumChar){
  int size = size1 + size2;
  Record * records = new Record[size];

  int atrNum = atoi(atrNumChar);

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

  /* Copy the remaining elements of L[], if there are any */
  while (i < size1) {
    records[k] = records1[i];
    i++;
    k++;
  }
 
  /* Copy the remaining elements of R[], if there are any */
  while (j < size2)
  {
    records[k] = records2[j];
    j++;
    k++;
  } 

  return records;
}

Record* split(char * filename, char * atrNumChar, int depth, int low, int numOfrecords, pid_t rootPid, int totalRecords, int random) {
  long int pidL;
  pid_t pid;
  
  pid_t cpid;
  pid_t cpid2;
  char buff[20];
  int fd1[2];  // Used to store two ends of first pipe
  int fd2[2];  // Used to store two ends of second pipe
  // printf("low: %d numofRecord: %d, depth: %d\n", low, numOfrecords, depth );
  Record * recordsMerged;
  int size = numOfrecords - (numOfrecords/2+low/2);
  int mid = numOfrecords/2+low/2;

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

    if(random){
       cout<<"Mid if rand: " << mid<<"  "<< low <<":"<<low+size<<endl;

      mid = rand() % numOfrecords + low;
    }
    
    if((cpid = fork()) == 0) {
      //Child 1
      double t1, t2, cpu_time; struct tms tb1, tb2; double ticspersec;
      ticspersec = (double) sysconf(_SC_CLK_TCK);
      t1 = (double) times(&tb1);

      Record* recordsMerged1 = split(filename, atrNumChar, depth - 1, low, mid, rootPid, totalRecords, random);
      close(fd1[0]);
      printToPipe(recordsMerged1, size, fd1[1]);

      t2 = (double) times(&tb2);
      cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
      reportTime("A spliter/merger node running", (t2 - t1) / ticspersec, cpu_time / ticspersec);
      // printf("Run time of a spliter/merger node was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n", (t2 - t1) / ticspersec, cpu_time / ticspersec);
  
      exit(0);
    } else {
      //Parent
      if((cpid2 = fork()) == 0) {
        //Child 2
        double t1, t2, cpu_time; struct tms tb1, tb2; double ticspersec;
        ticspersec = (double) sysconf(_SC_CLK_TCK);
        t1 = (double) times(&tb1);

        
        Record* recordsMerged2 = split(filename, atrNumChar, depth - 1, mid, numOfrecords, rootPid, totalRecords, random); 
        close(fd2[0]);
        printToPipe(recordsMerged2, size, fd2[1]);

        t2 = (double) times(&tb2);
        cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
        reportTime("A spliter/merger node running", (t2 - t1) / ticspersec, cpu_time / ticspersec);
        // printf("Run time of a spliter/merger node was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n", (t2 - t1) / ticspersec, cpu_time / ticspersec);
  
        exit(0);
      } else {
        close(fd1[1]); 
        close(fd2[1]);

        Record* records1 = readFromPipe(fd1, size);
        Record* records2 = readFromPipe(fd2, size);

        waitpid(cpid, NULL, 0);
        waitpid(cpid2, NULL, 0);

        recordsMerged = merge(records1, size, records2, size, atrNumChar);
        // recordsMerged = records1;
        // printRecords(recordsMerged, 2 * size);
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

    if(random){
      cout<<"Mid if rand: " << mid<<"  "<< low <<":"<<low+size<<endl;
      mid = rand() % size + low;
    }


    if((cpid = fork()) == 0) {
      //child 1
      close(fd1[0]);
      callExec(filename, low, mid, atrNumChar, fd1[1], rootPid, totalRecords);
      perror("exec failure.. ");
      exit(1);
    } else {
      //parent process 
      if((cpid2 = fork()) == 0){
        //child 2 
        // close reading end
        close(fd2[0]);
        callExec(filename, mid, numOfrecords, atrNumChar, fd2[1], rootPid, totalRecords);
        perror("exec failure.. ");
        exit(1);
      } else {
        // Parent
        //Close writing ends
        close(fd1[1]); 
        close(fd2[1]);

        // printf("Num of Records: %d, low: %d\n", numOfrecords, low);
        // printf("Upper range bound: %d\n", numOfrecords);
        Record* records1 = readFromPipe(fd1, size);
        // printf("Pringting %d sorted records in main!! YEEEY!\n", size);
        // printRecords(records1, size);
        
        Record* records2 = readFromPipe(fd2, size);
        // printf("Pringting %d sorted records in main!! YEEEY!\n", size);
        // printRecords(records2, size);

        waitpid(cpid, NULL, 0);
        waitpid(cpid2, NULL, 0);

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
  char *outFile = "none";
  int random = 0;
  int numOfrecords;
  long lSize;
  FILE *fpb;
  int status;
  pid_t pid;
  int fd[2];

  //read the flags and get the parametors from the command line
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

  //Reset a file used to report time and output file
  char* timeFile = "reportedTime.txt";
  fclose(fopen(timeFile, "w"));
  fclose(fopen(outFile, "w"));

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
  // printf("Records found in file %d \n", numOfrecords);
  numOfrecords = 160;
  
  signal(SIGUSR1, handle_sigusr1);
  signal(SIGUSR2, handle_sigusr2); 
  signal(SIGCONT, handle_kill); 


  if (pipe(fd) == -1) {
    fprintf(stderr, "Pipe Failed" );
    exit(1);
  }

  //root has one child
  if((pid = fork()) == 0) {
    double sm0_t1, sm0_t2, sm0_cpu_time; struct tms sm0_tb1, sm0_tb2; double sm0_ticspersec;
    sm0_ticspersec = (double) sysconf(_SC_CLK_TCK);
    sm0_t1 = (double) times(&sm0_tb1);

    //child process
    Record * sortedRecords;
    close(fd[0]);
    sortedRecords = split(filename, atrNumChar, depth, 0, numOfrecords, getppid(), numOfrecords, random);
    printToPipe(sortedRecords, numOfrecords, fd[1]);    

    sm0_t2 = (double) times(&sm0_tb2);
    sm0_cpu_time = (double) ((sm0_tb2.tms_utime + sm0_tb2.tms_stime) - (sm0_tb1.tms_utime + sm0_tb1.tms_stime));

    reportTime("Spliter/Merger 0", (sm0_t2 - sm0_t1) / sm0_ticspersec, sm0_cpu_time / sm0_ticspersec);
    // printf("Spliter/Merger 0 time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n", (sm0_t2 - sm0_t1) / sm0_ticspersec, sm0_cpu_time / sm0_ticspersec);
    
    exit(0);
  } 

  close(fd[1]);
  Record* records = readFromPipe(fd, numOfrecords);
  waitpid(pid, NULL, 0);
  close(fd[0]);
  
  if(strcmp(outFile,"none") == 0 ){
    printRecords(records, numOfrecords);
  } else {
    printToFile(outFile, records, numOfrecords);
  }


  checkNumOfSignalsMissing((int)sh_s, (int)q_s, (int)b_s, depth, outFile);
  cout << "SH_S "<< sh_s << " Q_S " << q_s <<" B_S " << b_s << endl;
  
    //report time part 2
  // printf("%d\n", merge);
  t2 = (double) times(&tb2);
  cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
  reportTime("Turnaround", (t2 - t1) / ticspersec, cpu_time / ticspersec);

  reportAllTimestamps(timeFile, outFile);
  // printf("Turnaround time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n", (t2 - t1) / ticspersec, cpu_time / ticspersec);
  return 0;
}

