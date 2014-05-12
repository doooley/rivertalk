#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
//#include "wlog.h"

#define MAXDATASIZE 140
#define PORT "4991"
#define BACKLOG 10

int socksetup () {
  int sock, yes=1;
  struct addrinfo hints, *servinfo, *p;

	// INITIALISE their_addr
          memset (&hints, 0, sizeof(hints));
     	  hints.ai_family=AF_UNSPEC;
          hints.ai_socktype=SOCK_STREAM;
          hints.ai_flags=AI_PASSIVE; 
       	  if(0 != getaddrinfo(NULL, PORT, &hints, &servinfo))
            exit(1);

    for(p=servinfo; p!=NULL; p=p->ai_next) {
	if(-1==(sock=socket(p->ai_family, p->ai_socktype, p->ai_protocol)))
	  continue;
	if(-1==setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
	  continue;
	if(-1== bind(sock, p->ai_addr, p->ai_addrlen)){
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

void clrbuf(char* buffer){
  int i;
  for(i=0;i<strlen(buffer);i++)
    buffer[i]='\0';
}

void updateclients(fd_set *active_set, char* buffer, int sock){
  int i;
  printf("Sending message back to clients\n\t%s", buffer);
  for (i=0;i<FD_SETSIZE; i++)
    if(FD_ISSET(i, active_set) && i != sock){
      if(-1 == send(i, buffer, 140, 0)){
	printf("closing %d\n", i);
        close(i);
        FD_CLR(i, active_set);
      }
    }
  clrbuf(buffer);
}

int getmsg (int fd, char *buffer) {
  int numbytes;
  numbytes= recv(fd, buffer, MAXDATASIZE-1, 0);
  if(0 > numbytes)
    exit(EXIT_FAILURE);
  else if(0 == numbytes)
    return -1;
  else{
    printf("recv: %s\n", buffer);
    return 0;
    }
}

int main(){
  int sock, i,j, size;
  fd_set active_set, read_set;
  struct sockaddr_storage their_addr;
  char *buffer = (char *)malloc(MAXDATASIZE*sizeof(char));
  
  sock = socksetup();
      //  BACKLOG SET TO 1 HERE... is that ok?
  if(0 > listen(sock, 1)) exit(EXIT_FAILURE);
  
  FD_ZERO(&active_set);
  FD_SET(sock, &active_set);

  while(1){
    read_set=active_set;
    if(0 > select(FD_SETSIZE, &read_set, NULL, NULL, NULL)){
      exit(EXIT_FAILURE);
    }

    for(i=0;i<FD_SETSIZE;i++){
      if(FD_ISSET(i, &read_set)){
        if(i==sock){
	    printf("new connection! %d\n", i);
          int new_fd;
	  size=sizeof(their_addr); 
	  new_fd=accept(sock, (struct sockaddr *)&their_addr, &size);
	  if(0 > new_fd) exit(EXIT_FAILURE);
		// MAYBE RECORD CONNECTION DETAILS HERE??
		// see http://www.gnu.org/software/libc/manual/html_node/Server-Example.html
		//     fprintf (stderr,
		//         "Server: connect from host %s, port %hd.\n",
		//          inet_ntoa (clientname.sin_addr),
		//          ntohs (clientname.sin_port)
		//          );
	  FD_SET(new_fd, &active_set);
	} else {
	    if(0 > (j=getmsg(i, buffer))){
	      close(i);
	      FD_CLR(i, &active_set);
	    }
	    updateclients(&active_set, buffer, sock);
          }
	}
    }
  } 

  for(i=0;i<FD_SETSIZE;i++)
    if(FD_ISSET(i, &active_set)){
      close(i);
      FD_CLR(i, &active_set);
    }
  free(buffer);
return 0;
}
