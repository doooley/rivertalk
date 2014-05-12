#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ncurses.h>
//#include "wlog.h"

#define PORT "4991"
#define MAXDATASIZE 140
#define ADDR NULL

void clrbuf(char* buffer){
  int i;
  for(i=0;i<strlen(buffer);i++)
  buffer[i]='\0';
}

int socksetup () {
  int sock, yes=1;
  struct addrinfo hints, *servinfo, *p;

        // INITIALISE their_addr
          memset (&hints, 0, sizeof(hints));
          hints.ai_family=AF_UNSPEC;
          hints.ai_socktype=SOCK_STREAM;
          if(0 != getaddrinfo(NULL, PORT, &hints, &servinfo))
            exit(1);

    for(p=servinfo; p!=NULL; p=p->ai_next) {
        if(-1==(sock=socket(p->ai_family, p->ai_socktype, p->ai_protocol)))
          continue;
        if(-1== connect(sock, p->ai_addr, p->ai_addrlen)){
          close(sock);
          continue;
          }
      break;
      }
  if(p==NULL)
    exit(EXIT_FAILURE);

  freeaddrinfo(servinfo);

return sock;
}

int main(){
  int sockfd, numbytes, childproc;
  char *buffer = (char*) malloc(MAXDATASIZE*sizeof(char));
  sockfd=socksetup();

  childproc = fork();

  if(childproc >= 0){
    if(childproc > 0){	 //RECEIVING MESSAGES
      while((numbytes = recv(sockfd, buffer, MAXDATASIZE-1, 0)) >0) {
        buffer[numbytes]='\0';
        printf("%s", buffer);
        clrbuf(buffer);
        }
      } else	 	//SENDING MESSAGES
       while((fgets(buffer,140*sizeof(char),stdin ))!=NULL){
         if (send(sockfd, buffer, 140, 0) == -1)
		close(sockfd);
         clrbuf(buffer);
         }
  }

  close(sockfd);
  free(buffer);

return 0;
}
