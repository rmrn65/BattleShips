#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
typedef struct player_stuct {
	int xPos,yPos;
}Player;

char** file_in_char(FILE* fptr,char filename[20])
{
	char** string = (char**)malloc(100*sizeof(char*));
	char read_string[100];
	fptr = fopen(filename,"r");
	int i = 0;
 	while(fgets(read_string,sizeof(read_string),fptr) != NULL)
 	{
 		read_string[strlen(read_string) - 1] = '\0';
 		string[i] = malloc((strlen(read_string) + 1)*sizeof(char));
 		strcpy(string[i],read_string);
 		i++;
 	}
 	return string;
 }
 void mainScreen(char** ascii_art)
{
	for(int i = 0 ; i < 10 ; i ++)
	{
		move(i+1 ,1);
       	printw("%s",ascii_art[i]);
       	refresh();	
	}
}
void turnON(int i, int j,char** string)
{
	attron(A_REVERSE); 
	mvprintw(j-1, i-1, "%c", string[i][j]);
	attroff(A_REVERSE);
}
void turnOFF(int i, int j,char** string)
{
	mvprintw(j-1,i-1,"%c",string[i][j]);
}
int main(int argc,char*argv[])
{
FILE* board;
char** mesg;
int row,col;
initscr();
noecho();
cbreak();
keypad(stdscr,TRUE);
mesg = file_in_char(board,"board.txt");
mainScreen(mesg);
int ch;
int i=9,j=10;
int x=20,y=10;

move(i,j);
while((ch = getch()) != 'q')
{
	switch(ch)
	{
		case KEY_UP:
		if(i-1 > 0)
		{	
			move(i-1,j);
			i = i - 1;
		}
		break;
		case KEY_DOWN:
		if(i+1 > 0)
		{	
			move(i+1,j);
			i = i + 1;
		}
		break;
		case KEY_RIGHT:
		if(j < 20)
		{	
			move(i,j+2);
			j = j + 2;
		}
		break;
		case KEY_LEFT:
		if(j-2 > 0)
		{	
			move(i,j-2);
			j = j - 2;
		}
		break;
		case '\n':
		if(mesg[i-1][j-1] == 'X' )
		{
			attron(A_REVERSE);
			printw("X");
			ok[i][j] = 1; 
		}

		break;
	}
	
}
endwin();
return 0;
}