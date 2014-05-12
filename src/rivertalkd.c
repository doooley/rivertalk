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
#include "wlog.h"

#define MAXDATASIZE 140
#define PORT "4991"
#define BACKLOG 10

void sigchld_handler(int s) {
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

void clrbuf(char* buffer){
  int i;
  for(i=0;i<strlen(buffer);i++)
  buffer[i]='\0';
}


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(){
  struct sockaddr_storage their_addr;
  struct addrinfo hints, *servinfo, *p;
  int sockfd, yes=1, addr_size, new_fd, numbytes;
  char s[INET6_ADDRSTRLEN], *buffer;
    buffer= malloc(140*sizeof(char));

  FILE* wlogfp;
wlogfp=setwlogfile("s");
loginfo(wlogfp,"INFO", "something's going through maybe...", "now??");

        // SET UP addrinfo STRUCT hints TO BE FED INTO servinfo
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
       loginfo(wlogfp, "ERROR","something in getaddrinfo() fucked up", NULL);
    exit(1);
  }

  // LOOP THROUGH servinfo AND BIND TO FIRST 
  for (p=servinfo; p!=NULL; p=p->ai_next) {
    if(-1 == (sockfd=socket(p->ai_family, p->ai_socktype,p->ai_protocol)))
        continue;
    if(-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
        continue;
    if(-1 == (bind(sockfd, p->ai_addr, p->ai_addrlen))){
        close(sockfd);
        continue;
    }
  break;
  }
if(p==NULL){
loginfo(wlogfp, "ERROR"," failed to bind", NULL);
exit(1);}
        
freeaddrinfo(servinfo); // all done with this structure

if(listen(sockfd, BACKLOG)==-1) {
loginfo(wlogfp, "ERROR","failed to listen", NULL);
exit(1);
}

// REAP DEAD PROCESSES 
struct sigaction sa;
sa.sa_handler = sigchld_handler; // reap all dead processes
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;
if (sigaction(SIGCHLD, &sa, NULL) == -1) {
loginfo(wlogfp, "ERROR","reaping dead processes (sigaction)", NULL);
exit(1);}
/* 
 * (no idea how this works) 
 */

loginfo(wlogfp, "INFO","waitin on connections....", NULL);
    while(1){
       addr_size = sizeof their_addr;
       new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
      
      if(new_fd==-1)
        continue;
      
      inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
      loginfo(wlogfp, "INFO","got a connection!!", NULL);
      
      if(!fork()){
        loginfo(wlogfp, "INFO", "closing the connection..", NULL);
        close(sockfd);

           //RECEIVING MESSAGES
   while((numbytes = recv(new_fd, buffer, MAXDATASIZE-1, 0)) >0) {
      buffer[numbytes]='\0';
      loginfo(wlogfp, "INFO", "Received message:", buffer);

          if (send(new_fd, buffer, 140, 0) == -1)
            loginfo(wlogfp, "ERROR", "didnt send the message. :(", NULL);
      clrbuf(buffer);
}

        close(new_fd);
        exit(0);
      }
      close(new_fd);
    }
    
free(buffer);
fclose(wlogfp);
return 0;
}
