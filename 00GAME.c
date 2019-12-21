#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <panel.h>
#define MAX_LEN 1000
typedef struct _win_border_struct {
	chtype 	ls, rs, ts, bs, 
	 	tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct {

	int startx, starty;
	int height, width;
	WIN_BORDER border;
	WINDOW* window;
}WIN;
//functii
char** file_in_char(FILE* fptr);
void init_main_win(WIN* win,int h,int w,int lines,int cols);
void create_box(WIN *p_win);
void mainScreen(WIN* win,char** ascii_art);
void print_menu(WINDOW *menu_win, int highlight,char* choices[3],int n_choices);
void destroy_win(WINDOW* local_win);//07Win.c
void newGame(WINDOW* win,WIN* main_win);

WINDOW* create_newwin(int height,int width,int starty,int startx);

//main
int main()
{
    FILE *ascii;
    PANEL *my_panels[3];
    PANEL *top; 
    char** ascii_art;
    char* options[3]={"New Game","Resume Game","Quit"};
    int n_options = 3;
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
	// meniul cu optiuni
	WINDOW* Menu_win;
	refresh();
	Menu_win = create_newwin(10,20,(LINES-5)/2,(COLS - 10)/2);
	print_menu(Menu_win,highlight,options,3);
	// sfarsit meniu cu optiuni
	// interfata jocului
	WINDOW* Game_win;
	// sfarsit alinterfetei jocului
	my_panels[0] = new_panel(Main_win.window);
	my_panels[1] = new_panel(Menu_win);
	my_panels[2] = new_panel(Game_win);
	//********
	set_panel_userptr(my_panels[1], my_panels[2]);
	set_panel_userptr(my_panels[2], my_panels[0]);

	update_panels();
	doupdate();
//	navigate(&Main_win,Menu_win,Game_win,highlight,options,3);
	int ch;
	int exit = 0;
	top = my_panels[3];
	while((ch = getch()))
	{
		switch(ch)
		{
			case KEY_UP:
				if(highlight > 1)
	 				highlight--;
	 			print_menu(Menu_win,highlight,options,n_options);
				break;	
			case KEY_DOWN:
				if(highlight < n_options)
	 				highlight++;
	 			print_menu(Menu_win,highlight,options,n_options);
				break;	
			case '\n'://KEY_ENTER n-a mers
				if(highlight == 1)
			 	{
					newGame(Game_win,&Main_win);
					top = (PANEL*)panel_userptr(top);
					top_panel(top);

			 	}
			 	else if(highlight == 3)
				exit = 1;
				break;
		}	
		update_panels();
		doupdate();
		if(exit == 1)
			break;
	}
	endwin();
	
    return 0;
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
	win->window = newwin(win->height,win->width,win->starty+1,win->startx);
	// box(win->window,0,0);
}
void mainScreen(WIN* win,char** ascii_art)
{
	move((LINES - win->height)/2,(COLS - win->width)/2);
	for(int i = 0 ; i < 6 ; i ++)
	{
		move((LINES - win->height)/2+i-6,(COLS - win->width)/2+16);
       	printw("%s",ascii_art[i]);
       	refresh();	
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
WINDOW* create_newwin(int height,int width,int starty,int startx) 
{
	WINDOW* local_win;
	local_win = newwin(height,width,starty,startx);
	box(local_win,0,0);//
	wrefresh(local_win);
	return local_win;
}
void destroy_win(WINDOW* local_win)//07Win.c
{
	wborder(local_win,' ',' ',' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}
void newGame(WINDOW* win,WIN* main_win)
{
	win = create_newwin(31,110,main_win->starty+1,main_win->startx);
	mvwprintw(win,2,2,"Hello New Game");
	wrefresh(win);
}
void create_box(WIN *p_win)
{	int i, j;
	int x, y, w, h;

	x = p_win->startx;
	y = p_win->starty;
	w = p_win->width;
	h = p_win->height;

	//mvaddch(y, x, p_win->border.tl);
	//mvaddch(y, x + w, p_win->border.tr);
	//mvaddch(y + h, x, p_win->border.bl);
	//mvaddch(y + h, x + w, p_win->border.br);
	mvhline(y, x + 1, p_win->border.ts, w - 1);
	mvhline(y + h + 2, x + 1, p_win->border.bs, w - 1);
	mvvline(y + 2 , x-1, p_win->border.ls, h - 1);
	mvvline(y + 2, x + w, p_win->border.rs, h - 1);
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