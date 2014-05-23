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

#define PORT "4991"
#define MAXDATASIZE 140

void clrbuf(char* buffer){
  int i;
  for(i=0;i<strlen(buffer);i++)
  buffer[i]=' ';
}

int socksetup (char *addr) {
  int sock, yes=1;
  struct addrinfo hints, *servinfo, *p;

        // INITIALISE their_addr
          memset (&hints, 0, sizeof(hints));
          hints.ai_family=AF_UNSPEC;
          hints.ai_socktype=SOCK_STREAM;
          if(0 != getaddrinfo(addr, PORT, &hints, &servinfo))
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

void startscreen(){
  int row, col, my, mx;
  char msg[]="rivertalk.";

  initscr();
  raw();
  noecho();

  getmaxyx(stdscr,row,col);
  my=(int)row*0.5;
  mx=(int)(col - 5)*0.5;

  attron(A_BOLD);
  mvprintw(my, mx, msg);
  attroff(A_BOLD);
  refresh();

  getch();
  clear();
  refresh();
  endwin();
}

int main(int argc, char *argv[]){
  int sockfd, numbytes, childproc;
  int y, x;
  char *buffer = (char*) malloc(MAXDATASIZE*sizeof(char));
  char *addr = NULL;
    if(argc >= 2){
      addr=argv[1];
	}
  sockfd=socksetup(addr);

    startscreen(addr);
	//  SETTING UP ncurses WINDOWS
        WINDOW *in, *out;
        initscr();
        cbreak();
	echo();
        in = newwin(4, 0, LINES-4, 0);
          box(in, 0, 0);
        out = newwin(10, 0, 0, 0);
	scrollok(in, TRUE);
	scrollok(out, TRUE);


  childproc = fork();

  if(childproc >= 0){
    if(childproc > 0){	 //RECEIVING MESSAGES
      while((numbytes = recv(sockfd, buffer, MAXDATASIZE-1, 0)) >0) {
		if(0 < strlen(buffer))
            wprintw(out, "%s\n", buffer);
	    wrefresh(out);
	    wrefresh(in);
	    wmove(in, 1, 1);
        }
      } else	 	//SENDING MESSAGES
    while(1){
        mvwgetstr(in,1, 1, buffer);
	wmove(in, 1, 1);
        wclrtoeol(in);
         if (send(sockfd, buffer, 140, 0) == -1)
		close(sockfd);
         }
  }

wgetch(in);
delwin(in);
delwin(out);
endwin();

  close(sockfd);
  free(buffer);

return 0;
}
