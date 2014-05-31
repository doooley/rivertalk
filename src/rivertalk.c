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
#define USERNAMESIZE 15


void clrbuf(char* buffer){
  int i;
  for(i=0;i<strlen(buffer);i++)
  buffer[i]='\0';
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


char* parsemsg(char* user, char *msg, int userlength){
  int i;
  char* prefix = (char*) calloc((userlength + 4),sizeof(char));
  char* all = (char*) calloc((MAXDATASIZE + USERNAMESIZE + 4),sizeof(char));
  int msglength=strlen(msg);

   strncat(prefix, user, userlength);
   prefix[userlength]=' ';
   strncat(all, prefix, userlength+1);
   strncat(all, msg, msglength);

  free(prefix);

return all;
}

int getuser(char* msg){
  int i;
  for(i=0;msg[i]!=' '; i++);
  return i;
}

char* startscreen(){
  int row, col, my, mx, inx, iny;
  char *user=(char*)malloc(13*sizeof(char));
  char msg[]="rivertalk.";

  initscr();
  echo();

  getmaxyx(stdscr,row,col);
  my=(int)row*0.5;
  mx=(int)(col - 10)*0.5;
iny= my+2;
inx=mx-7;

  attron(A_BOLD);
  mvprintw(my, mx, msg);
  attroff(A_BOLD);
  refresh();
    getch();
  mvprintw(iny, inx, "Username: ");
  refresh();
mvgetstr(iny, inx+ 10, user);
  clear();
  refresh();
  endwin();
 return user;
}


int main(int argc, char *argv[]){
  int sockfd, numbytes, childproc;
  int y, x , i , L, index;
  char *msg, *user, *buffer = (char*) malloc((USERNAMESIZE + MAXDATASIZE + 4)*sizeof(char));
  char *addr = NULL;
    if(argc >= 2){
      addr=argv[1];
	}
  sockfd=socksetup(addr);

    user=startscreen();
    L=strlen(user);

	//  SETTING UP ncurses WINDOWS
        WINDOW *in, *out;
        initscr();
        cbreak();
	echo();
        in = newwin(4, 0, LINES-4, 0);
        out = newwin(LINES-4, 0, 0, 0);
	scrollok(in, TRUE);
	scrollok(out, TRUE);


  childproc = fork();

  if(childproc >= 0){
    if(childproc > 0){	 //RECEIVING MESSAGES
      while((numbytes = recv(sockfd, buffer, (USERNAMESIZE + MAXDATASIZE + 4), 0)) >0) {
		if(0 < strlen(buffer)){
                  index=getuser(buffer);
                  wattron(out, A_BOLD);
                  for(i=0;i<index;i++)
                    wprintw(out, "%c", buffer[i]);
                  wattroff(out, A_BOLD);
                  wprintw(out, "%s\n", buffer + index);
                }
	    wrefresh(out);
	    wrefresh(in);
	    wmove(in, 0, 0);
        }
      } else	 	//SENDING MESSAGES
    while(1){
        mvwgetstr(in,0, 0, buffer);
	wmove(in, 0, 0);
        werase(in);
        wrefresh(in);
        msg=parsemsg(user, buffer, L);
         if (send(sockfd, msg, MAXDATASIZE + USERNAMESIZE + 4, 0) == -1)
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
