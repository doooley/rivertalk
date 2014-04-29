#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "wlog.h"


FILE* setwlogfile(char *serverClient){

 struct timeval tv;
 struct tm* ptm;
int i;
 
 char date_str[20];
 char logfileName[29];
	for(i=0;i<29;i++)
	  logfileName[i]='\0';

 FILE* wlogfp ;
 
  gettimeofday ( &tv, NULL );
  ptm= localtime(&tv.tv_sec);
  strftime(date_str, sizeof(date_str) , "%Y-%m-%d", ptm);

    if(0==strncmp(serverClient,"c",1))
      strncpy(logfileName, "wlogfileClient", 14);
      else
        strncpy(logfileName, "wlogfileServer", 14);

    strncat(logfileName,date_str,10);
    strncat(logfileName,".log\0",4);

    if((wlogfp=fopen(logfileName, "a"))==NULL){
      printf("ERROR: log file didn't open. wtf?\n");
	  exit(1);}

return wlogfp;
}


void loginfo (FILE* wlogfile, char* typ, char* msg, char* var){

  struct timeval tv;
  struct tm* ptm;

  char time_string[40];
  long ms;

  gettimeofday ( &tv, NULL );
  ptm= localtime(&tv.tv_sec);
  strftime(time_string, sizeof(time_string) , "%Y.%m.%dT%H:%M:%S", ptm);

  ms= tv.tv_usec /1000.;

 fprintf(wlogfile, "<->%s.%03ld ### ", time_string, ms);
 fprintf(wlogfile, "%s ### ", typ);
 fprintf(wlogfile, "%s ", msg);
  if(var != NULL)
    fprintf(wlogfile, "%s ",var);
 fprintf(wlogfile, "###\n");
}
