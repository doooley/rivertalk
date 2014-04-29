#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "wlog.h"

#define PORT "4991"
#define MAXDATASIZE 140

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void clrbuf(char* buffer){
  int i;
  for(i=0;i<strlen(buffer);i++)
  buffer[i]='\0';
}


int main(int argc, char *argv[]){
  struct addrinfo hints, *servinfo, *p;
  int sockfd, numbytes;
  char *buffer = (char*) malloc(MAXDATASIZE*sizeof(char));
  char s[INET6_ADDRSTRLEN];

    FILE* wlogfp=setwlogfile("c");

	// SET UP addrinfo STRUCT hints TO BE FED INTO servinfo
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((getaddrinfo("10.152.20.95", "4991", &hints, &servinfo)) != 0) {
	loginfo(wlogfp, "ERROR", "something in getaddrinfo() fucked up", NULL);
    exit(1);
  }

// loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        loginfo(wlogfp, "ERROR", "client's socket no good.", NULL);
        continue;
      }
      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        loginfo(wlogfp, "ERROR", " connecting didn't work out here.", NULL);
        continue;
      }
      break;
    }

if (p == NULL) {
  loginfo(wlogfp, "ERROR","failed to connect to server", NULL);
  return 2;
}

inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
loginfo(wlogfp, "INFO", "connecting to server at", s);
  freeaddrinfo(servinfo);

  //if ((numbytes = recv(sockfd, buffer, MAXDATASIZE-1, 0)) == -1) {
    //loginfo(wlogfp, "ERROR","receiving didn't work", NULL);
     //exit(1);
  //}
   while((numbytes = recv(sockfd, buffer, MAXDATASIZE-1, 0)) >0) {
      buffer[numbytes]='\0';
      loginfo(wlogfp, "INFO", "Received message:", buffer);
      printf("%s", buffer);
      clrbuf(buffer);
    }
  close(sockfd);
fclose(wlogfp);
  free(buffer);
return 0;
}
