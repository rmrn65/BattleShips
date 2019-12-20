#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LEN 1000
typedef struct _win_border_struct {
	chtype 	ls, rs, ts, bs, 
	 	tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct {

	int startx, starty;
	int height, width;
	WIN_BORDER border;
	//WINDOW* win;
}WIN;
//functii
char** file_in_char(FILE* fptr);
void init_main_win(WIN* win,int h,int w,int lines,int cols);
void print_win_params(WIN *p_win);
void create_box(WIN *p_win);
void mainScreen(WIN* win,char** ascii_art);
void mainScreenOp(WIN* win,char* string[3],int nr_op);
void menu_screen_navigation(WIN *menu_win, int highlight);
void print_menu(WINDOW *menu_win, int highlight,char* choices[3],int n_choices);
void navigate(WINDOW *menu_win,int highlight,char* choices[3],int n_choices);
void destroy_win(WINDOW* local_win);//07Win.c

WINDOW* create_newwin(int height,int width,int starty,int startx);

//main
int main()
{
    FILE *ascii;
    char** ascii_art;
    char* options[3]={"New Game","Resume Game","Quit"};
    int highlight = 1;
  	int row,col;
	initscr();
	clear();
	noecho();
	cbreak();
	keypad(stdscr,TRUE);
	ascii_art = file_in_char(ascii);
	WIN Main_win;
	init_main_win(&Main_win,30,110,LINES,COLS);
	create_box(&Main_win);
	mainScreen(&Main_win,ascii_art);
	WINDOW* Menu_win;
	refresh();
	Menu_win = create_newwin(10,20,(LINES-5)/2,(COLS - 10)/2);
	print_menu(Menu_win,highlight,options,3);
	navigate(Menu_win,highlight,options,3);
	endwin();
    return 0;
}
void mainScreen(WIN* win,char** ascii_art)
{
	move((LINES - win->height)/2,(COLS - win->width)/2);
	for(int i = 0 ; i < 6 ; i ++)
	{
		move((LINES - win->height)/2-3+i,(COLS - win->width)/2+16);
       	printw("%s",ascii_art[i]);
       	refresh();	
	}
}
void mainScreenOp(WIN* win,char* string[3],int nr_op)
{
	move((LINES - win->height)/2,(COLS - win->width)/2);
	for(int i = 0;i < nr_op;i++)
	{
		move((LINES - win->height)/2+i + 1,(COLS - win->width)/2 + 4);
		printw("%s",string[i]);
	}

}
WINDOW* create_newwin(int height,int width,int starty,int startx) 
{
	WINDOW* local_win;
	local_win = newwin(height,width,starty,startx);
	box(local_win,0,0);//
	wrefresh(local_win);
	return local_win;
}
void navigate(WINDOW *menu_win,int highlight,char* choices[3],int n_choices)
{
	int ch;
	int exit = 0;
	while((ch = getch()))
	{
		switch(ch)
		{
			case KEY_UP:
				if(highlight > 1)
	 				highlight--;
	 			print_menu(menu_win,highlight,choices,n_choices);
				break;	
			case KEY_DOWN:
				if(highlight < n_choices)
	 				highlight++;
	 			print_menu(menu_win,highlight,choices,n_choices);
				break;	
			case '\n'://KEY_ENTER n-a mers
			if(highlight == 3)
				exit = 1;
			break;
		}	
		if(exit == 1)
			break;
	}
}
void print_menu(WINDOW *menu_win, int highlight,char* choices[3],int n_choices)
{
	int x, y, i;	
	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i)
	{	if(highlight == i + 1) /* High light the present choice */
		{	wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}
void destroy_win(WINDOW* local_win)//07Win.c
{
	wborder(local_win,' ',' ',' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}
void init_main_win(WIN* win,int h,int w,int lines,int cols)
{
	win->height = h;
	win->width = w;
	win->starty = (lines - win->height)/2;	
	win->startx = (cols - win->width)/2;
	win->border.ls = '#';
	win->border.rs = '#';
	win->border.ts = '*';
	win->border.bs = '*';
	win->border.tl = '+';
	win->border.tr = '+';
	win->border.bl = '+';
	win->border.br = '+';
}
void print_win_params(WIN *p_win)
{
#ifdef _DEBUG
	mvprintw(25, 0, "%d %d %d %d", p_win->startx, p_win->starty, 
				p_win->width, p_win->height);
	refresh();
#endif
}
void create_box(WIN *p_win)
{	int i, j;
	int x, y, w, h;

	x = p_win->startx;
	y = p_win->starty;
	w = p_win->width;
	h = p_win->height;

	mvaddch(y, x, p_win->border.tl);
	mvaddch(y, x + w, p_win->border.tr);
	mvaddch(y + h, x, p_win->border.bl);
	mvaddch(y + h, x + w, p_win->border.br);
	mvhline(y, x + 1, p_win->border.ts, w - 1);
	mvhline(y + h, x + 1, p_win->border.bs, w - 1);
	mvvline(y + 1, x, p_win->border.ls, h - 1);
	mvvline(y + 1, x + w, p_win->border.rs, h - 1);
	refresh();
}
char** file_in_char(FILE* fptr)
{
	char** string = (char**)malloc(6*sizeof(char*));
	char read_string[100];
	fptr = fopen("ASCII_art.txt","r");
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