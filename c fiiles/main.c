#include <stdio.h> /* printf() */
#include <sys/times.h>  /* times() */
#include <unistd.h>     /* sysconf() */
#include <string.h>   /* strncmp() */

int main(int argc, char* argv[])
{
  int i = 1;
  int depth = 1;
  char * filename = "";
  int atrNum = 0;
  char *outFile = "outputFile.txt";
  int random = 0;

  while(i < argc){
    char *flag = (argv[i]);
    if(strncmp (flag,"-d", 2) == 0){
      depth = atoi(argv[i+1]);
      printf("Depth level: %d\n", depth);
      i+=2;
    } else if(strncmp (flag,"-f", 2) == 0){
      filename = argv[i+1];
      printf("Filename: %s\n", filename);
      i+=2;
    } else if(strncmp (flag,"-a", 2) == 0){
      atrNum = atoi(argv[i+1]);
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

//FINDING THE NUMBER OF RECORDS IN FILE
  int numOfrecords;
  long lSize;

  // numOfrecords = end - start;
  // check number of records
  // fseek (fpb , 0 , SEEK_END);
  // lSize = ftell (fpb);
  // rewind (fpb);
  // numOfrecords = (int) lSize/ SIZEofBUFF;
  // printf("Records found in file %d \n", numOfrecords);

  double t1, t2, cpu_time; struct tms tb1, tb2; double ticspersec;
  ticspersec = (double) sysconf(_SC_CLK_TCK);
  t1 = (double) times(&tb1);


  t2 = (double) times(&tb2);
  cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
  printf("Run time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n", (t2 - t1) / ticspersec, cpu_time / ticspersec);

  return 0;
}
