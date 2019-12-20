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
}WIN;
//functii
char** file_in_char(FILE* fptr);
void init_main_win(WIN* win,int h,int w,int lines,int cols);
void print_win_params(WIN *p_win);
void create_box(WIN *p_win);
void mainScreen(WIN* win,char** ascii_art);
void mainScreenOp(WIN* win,char string[3][100],int nr_op);
void menu_screen_navigation(WIN *menu_win, int highlight);
//main
int main()
{
    FILE *ascii;
    char** ascii_art;
    char* options[3]={"New Game","Laderboard","Quit"};
    int highlight = 0;

  	int row,col;
	initscr();
	noecho();
	refresh();
	ascii_art = file_in_char(ascii);
	WIN Main_win;
	WINDOW* Menu_win;
	init_main_win(&Main_win,30,110,LINES,COLS);
	init_main_win(&Menu_win,4,15,LINES,COLS+7);
	create_box(&Main_win);
	//create_box(&Menu_win);
	mainScreen(&Main_win,ascii_art);
	//mainScreenOp(&Menu_win,options,3);
 	while(1)
 	{
 		
 		char d; 
 		d = getch();
 		if(d == 'q')
 		{
 			endwin();
 			break;
 		}
 	}
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
void init_main_win(WIN* win,int h,int w,int lines,int cols)
{
	win->height = h;
	win->width = w;
	win->starty = (lines - win->height)/2;	
	win->startx = (cols - win->width)/2;
	win->border.ls = '|';
	win->border.rs = '|';
	win->border.ts = '-';
	win->border.bs = '-';
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