#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include<time.h>
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
typedef struct _nave_struct {
	int len;
	int nr;
}SHIPS;
//functii
char** file_in_char(char* filename);
int** init_mat();
int verificare_OR(int y,int x,int len,int** interzis);
int verificare_VR(int y,int x,int len,int** interzis);
int nav_gen_OR(int** matrice,int** interzis,int len);
int nav_gen_VR(int** matrice,int** interzis,int len);
int** generate_enemy_board();

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
	ascii_art = file_in_char("ASCII_art.txt");

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
	// sfarsit alinterfetei jocului
//	navigate(&Main_win,Menu_win,Game_win,highlight,options,3);
	int ch;
	int exit = 0;
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
					WINDOW* Game_win;
					newGame(Game_win,&Main_win);
					init_main_win(&Main_win,30,110,LINES,COLS);
					create_box(&Main_win);
					mainScreen(&Main_win,ascii_art);
					print_menu(Menu_win,highlight,options,3);

			 	}
			 	else if(highlight == 3)
				exit = 1;
				break;
		}	
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
	//box(local_win,0,0);//
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
	//creez fereastra de joc	
	win = create_newwin(31,110,main_win->starty+1,main_win->startx);
	//printare my_board
	char** my_board;

	my_board = file_in_char("board.txt");
	int y,x,i,j;
	y = 10;
	x = 10;
	for(i = 0; i < 10; ++i)
	{		
		mvwprintw(win, y, x, "%s", my_board[i]);
		++y;
	}
	//printare enemy_board - gol
	char** enemy_field;
	enemy_field = file_in_char("enemy_board.txt");
	getmaxyx(win,y,x);
	y = 10;
	for(i = 0; i < 10; ++i)
	{		
		mvwprintw(win, y, x-30, "%s", enemy_field[i]);
		++y;
	}
	//generare enemy_board
	int** enemy_board = generate_enemy_board();
	keypad(win,TRUE);

	//move pe enemy_board
	int boardx = x-30,boardy = 10;
	int chr;
	boardx++;
	wmove(win,boardy,boardx);
	int** visited;
	visited = init_mat();
	while((chr = wgetch(win)) != 'q')
	{
		switch(chr)
		{
			case KEY_UP:
			if(boardy-1 >=10)
			{
				wmove(win,boardy - 1,boardx);
				boardy--;				
			}
				break;
			case KEY_DOWN:
			if(boardy+1 <y)
			{
				wmove(win,boardy+1,boardx);
				boardy++;
			}
			break;
			case KEY_RIGHT:
			if(boardx + 2 < x-10)
			{
				wmove(win,boardy,boardx + 2);
				boardx= boardx + 2;
			}
			break;
			case KEY_LEFT:
			if(boardx - 2 > x - 30)
			{
				wmove(win,boardy,boardx - 2);
				boardx = boardx - 2;
			}
			break;
			case '\n':
			if(enemy_board[boardy - 10 + 1][(boardx-80)/2 + 1] == 1 &&
			 visited[boardy - 10 + 1][(boardx-80)/2 + 1] == 0)
			{
				wprintw(win,"X");
				visited[boardy - 10 + 1][(boardx-80)/2 + 1] = 1;
			}
			else if((enemy_board[boardy - 10 + 1][(boardx-80)/2 + 1] == 0 &&
			 visited[boardy - 10 + 1][(boardx-80)/2 + 1] == 0))
			{
				wprintw(win,"-");
				visited[boardy - 10 + 1][(boardx-80)/2 + 1] = 1;
			}
			// enemy_attack();
			break;
		}
	}
	//if(enter && matrice[i][j] == 1 ) print X
	//mvwprintw(win,2,2,"Hello New Game");
	//wrefresh(win);

}
//RANDOMLY GENERATING ENEMY BOARD
int** init_mat()
{
	int** matrice;
	matrice = calloc(12,sizeof(int*));
	for(int i = 0; i < 12; i ++)
		matrice[i] = calloc(12,sizeof(int));
	return matrice;
}
int verificare_OR(int y,int x,int len,int** interzis)
{
	int i,j;
	int ok = 1;
	for( j = x; j < x + len ;j++ )
	{
		if(interzis[y][j] == 2)
		{	
			ok = 0;
			break;
		}
	}
	return ok;
}
int nav_gen_OR(int** matrice,int** interzis,int len)
{
	int i,j;
	int diry = rand()%10 + 1;
	int dirx = rand()%(10 - len - 1) + 1;//(10 - len - 1)
	int ok; 
	ok = verificare_OR(diry,dirx,len,interzis);
	if(ok == 1)
	{
		for( j = dirx; j < dirx + len ;j++ )
		{
			matrice[diry][j] = 1;
			interzis[diry][j] = 2;
			interzis[diry-1][j] = 2;
			interzis[diry+1][j] = 2;
		}
		interzis[diry][dirx-1] = 2;
		interzis[diry][j] = 2;
		interzis[diry+1][j] = 2;
		interzis[diry-1][j] = 2;
		interzis[diry+1][dirx-1] = 2;
		interzis[diry-1][dirx-1] = 2;	
	}
	return ok;
}

int verificare_VR(int y,int x,int len,int** interzis)
{
	int i,j;
	int ok = 1;
	for( i = y; i < y + len ;i++ )
	{
		if(interzis[i][x] == 2)
		{
			ok = 0;
			break;
		}
	}
	return ok;
}
int nav_gen_VR(int** matrice,int** interzis,int len)
{
	int i,j;
	int dirx = rand()%10 + 1;
	int diry = rand()%(10 - len - 1) + 1;//(10 - len - 1)
	int ok = verificare_VR(diry,dirx,len,interzis);
	if(ok == 1)
	{
		for( i = diry; i < diry + len ;i++ )
		{		
			matrice[i][dirx] = 1;
			interzis[i][dirx] = 2;
			interzis[i][dirx-1] = 2;
			interzis[i][dirx+1] = 2;
		}
		interzis[diry-1][dirx] = 2;
		interzis[i][dirx] = 2;
		interzis[i][dirx+1] = 2;
		interzis[i][dirx-1] = 2;
		interzis[diry-1][dirx+1] = 2;
		interzis[diry-1][dirx-1] = 2;	
	}
	return ok;
}
int** generate_enemy_board()
{
	int** matrice = init_mat();
	int** interzis = init_mat();
	//char** matrice_char = init_mat_char();
	SHIPS nave[4];
	//numar nave
	nave[0].nr = 1;
	nave[1].nr = 2;
	nave[2].nr = 3;
	nave[3].nr = 3;
	//lungime nave
	nave[0].len = 4;
	nave[1].len = 3;
	nave[2].len = 2;
	nave[3].len = 1;
	int index_nave = 0;
	int i,j;
	srand(time(0)); 
	int dir_rand;
	int verif_if_placed;
	while(index_nave<4)//struct de nave, fiecare cu lungimea ei,fiecare cate sunt
	{	
		//luam fiecare tip de nave la rand;
		dir_rand = rand()%2;
		if(dir_rand == 1)//orizontal
		{
		verif_if_placed = nav_gen_OR(matrice,interzis,nave[index_nave].len);
		}
		else//vertical
		{
		verif_if_placed = nav_gen_VR(matrice,interzis,nave[index_nave].len);
		}
		if(verif_if_placed == 1)
			nave[index_nave].nr --;
		
		if(nave[index_nave].nr == 0)
		index_nave++;
	}
	return matrice;
}
//*finish o randomly generate
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
char** file_in_char(char* filename)
{
	FILE* fptr;
	char** string = (char**)malloc(11*sizeof(char*));
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