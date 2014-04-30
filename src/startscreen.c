#include <stdio.h>
#include <curses.h>

int main(){
int row, col, my, mx;
char msg[]="wlog.";

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
endwin();

return 0;
}
