#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> // folosesc biblioteca pentru randomizare
#include <unistd.h> // folosesc biblioteca pentru sleep
#define MAX_LEN 1000
//folosesc stuctura pentru customizarea marginii ferestrei cu jocul
typedef struct _win_border_struct {
	chtype 	ls, rs, ts, bs, 
	 	tl, tr, bl, br;
}WIN_BORDER;
//folosesc structura pentru a construi o fereastra
typedef struct _WIN_struct {

	int startx, starty;//coordonatele x si y de unde incepe fereastra
	int height, width;//ce inaltime si grosime are
	WIN_BORDER border;
	WINDOW* window;
}WIN;
//structura de nave
typedef struct _nave_struct {
	int len;
	int nr; //--> folosit doar pentru generare
	int startx;// coordonata x a pivotului navei
	int starty;// coordonata y a pivotului navei
	int direction; // 0 0rizontal 1 vertica1
	int damaged; // numara cate portiuni din structura navei au fost atacate
}SHIPS;
//structura jucatorului
typedef struct _player_struct{
	SHIPS* ships;//navele jucatorului
	int** board;//tabla sa. Matrice cu 1 pe post de nava si 0 gol
	int** visited;//ce casute a incercat jucatorul de pe tabla oponenta
	int hit;//de cate ori a lovit nava inamica
	int count_destroyed;//numar cate entitati a distrus jucatorul
}PLAYER;
//functii
// transfer un fisier text cu numele filename intr-o matrice
// de siruri de caractere
char** file_in_char(char* filename);
// initializez dinamic memoria unei matrici de numere
int** init_mat();
// Urmatoarele 4 functii sunt folosite pentru crearea aleatoare a 
// navelor pe tabla de joc
// #1 verific pe orizontala daca se mai pot introduce nave
int verificare_OR(int y,int x,int len,int** interzis);
// #2 verific pe verticala daca se mai pot introduce nave
int verificare_VR(int y,int x,int len,int** interzis);
// #3 generez pe orizontala o nava
int nav_gen_OR(int** matrice,int** interzis,int len,SHIPS* nave);
// #4 generez pe verticala o nava
int nav_gen_VR(int** matrice,int** interzis,int len,SHIPS* nave);
// Generez matricea inamicului cu navele 
int** generate_enemy_board();
// Generez matricea mea cu navele
int** my_board_in_01(char* my_board_text);

// Pnetru meniu (si tot ce e vizibil)
// Initializez fereastra principala
void init_main_win(WIN* win,int h,int w,int lines,int cols);
// Printez marginile ferestrei in care se desfasoara jocul
void create_box(WIN *p_win);
// Printez fereastra principala
void mainScreen(WIN* win,char** ascii_art);
// Printez fereastra cu optiunile 
void print_menu(WINDOW *menu_win, int highlight,char* choices[3],int n_choices);
// Initializez un nou joc - in caz ca apas New Game din fereastra Optiuni
WINDOW* initNewGame(WIN* main_win,char* my_board_text);
// Desfasor rularea jocului
void Game(WINDOW* win,WIN* main_win,PLAYER* player,int* count_visited);
// Creez o noua fereastra de inaltime data cu punctul pivot de coordonate
// starty si startx
WINDOW* create_newwin(int height,int width,int starty,int startx);
// Din matricea de 0 si 1 , unde 1 reprezinta o bucata de nava,
// creez nave ca entitati individuale cu punte pivot si orientari
SHIPS* in_entities(int** matrix);
// identific carei nave apartine bucata lovita in coordonatele y si x
int which_boat(int y,int x,PLAYER* player,int id_player);
// printez o tabla grafica in functie de o matrice matrix primita ca argument
void from_matrix_to_board(int** matrix,WINDOW* win);
// generez tabla de joc in functie de structurile de nave
int** generate_board_by_struct(SHIPS* nave,int nr_nave);
// functie pentru rearanjarea tablei proprii
void rearange_board(PLAYER* player);
//eliberare memorie
void free_mem(PLAYER* p,char** c,int ok);
int check_files(char* my_board_text);


int main(int argc,char* argv[])
{

    int ok;
  	ok = check_files(argv[1]);
  	if(ok == 1){
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
	//salvez in vectorul de siruri de caractere ascii-artul "BATTLESHIPS"
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_BLUE, COLOR_WHITE);
	ascii_art = file_in_char("ASCII_art.txt");
	WIN Main_win;
	// initializez fereastra principala
	init_main_win(&Main_win,30,110,LINES,COLS);
	create_box(&Main_win);
	mainScreen(&Main_win,ascii_art);
	// meniul cu optiuni
	WINDOW* Menu_win;
	refresh();
	Menu_win = create_newwin(10,20,(LINES-5)/2,(COLS - 10)/2);
	print_menu(Menu_win,highlight,options,3);
	// sfarsit meniu cu optiuni
	int ch;
	int exit = 0;
	WINDOW* Game_win;
	PLAYER* player;
	player = malloc(2*sizeof(PLAYER));
	int count_visited;
	int ENTERED = 0;
	//pentru navigarea prin opituni
	// Highlight decide ce optiune e evidentiata pentru ca utilizatorul sa
	// stie ce optiune alege
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
					// initializez structura ambilor jucatori
					// 1 este calculatorul si 0 e jucatorul-om
					player[0].visited = init_mat();
					player[1].visited = init_mat();
					player[0].board = my_board_in_01(argv[1]);
					player[1].board = generate_enemy_board();
					player[0].ships = in_entities(player[0].board);
					player[1].ships = in_entities(player[1].board);					
					player[0].hit = 0;
					player[1].hit = 0;
					player[1].count_destroyed = 0;
					player[0].count_destroyed = 0;
					//numar cate bucati din tabla au fost vizitate
					count_visited = 0;
					//initializez noul joc
					Game_win = initNewGame(&Main_win,argv[1]);//modificat
					//intru in joc
					Game(Game_win,&Main_win,player,&count_visited);
					// reinitializez fereastra in care se desfasoara jocul
					init_main_win(&Main_win,30,110,LINES,COLS);
					create_box(&Main_win);
					mainScreen(&Main_win,ascii_art);
					print_menu(Menu_win,highlight,options,3);
					// pentru a preveni intrarea in resume game fara
					// a fi jucat niciun joc pana acum
					ENTERED = 1;
			 	}
			 	else if(highlight == 2 && ENTERED == 1)
			 	{
			 		// revin la starea jocului de unde a ramas 
			 		scr_restore("savefile.txt");
			 		refresh();
			 		//intru in joc
			 		Game(Game_win,&Main_win,player,&count_visited);
					// initializez ferastra
					init_main_win(&Main_win,30,110,LINES,COLS);
					create_box(&Main_win);
					mainScreen(&Main_win,ascii_art);
					print_menu(Menu_win,highlight,options,3);
			 	}
			 	else if(highlight == 3)
				{
					exit = 1;
				}
				// ies din joc
				break;
		}	
		if(exit == 1)
			break;
	}
	endwin();//se inchide fereastra cu intreg jocul   
    free_mem(player,ascii_art,ENTERED);
	}
    return 1;
}
int check_files(char* my_board_text)
{
	FILE* f;
    if((f = fopen("savefile.txt","r")))
    {
    	fclose(f);
    }
    else 
    {
    	printf("Nu exista fisierul savefile.txt\n");
    	return 0;
    }
    if((f = fopen("ASCII_art.txt","r")))
    {
    	fclose(f);
    }
    else
    {
    	printf("Nu exista fisierul ASCII_art.txt");
    	return 0;
    }
    if((f = fopen(my_board_text,"r")))
    {
    	fclose(f);
    }
    else
    {
    	printf("Nu ai dat argumentul cu tabla ta de joc");
    	return 0;
    }
    if((f = fopen("enemy_board.txt","r")))
    {
    	fclose(f);
    }
    else
    {
    	printf("Nu exista fisierul enemy_board.txt");
    	return 0;
    }
    return 1;
    //if((file = fopen))
}
void free_mem(PLAYER* p,char** c,int ok)
{
	int i;
	if(ok == 1)
	for(i = 0 ; i < 12 ;i ++)
	{
		free(p[0].visited[i]);
		free(p[0].board[i]);
		free(p[1].visited[i]);
		free(p[1].board[i]);
	}
	free(p[0].visited);
	free(p[0].board);
	free(p[0].ships);
	free(p[1].visited);
	free(p[1].board);
	free(p[1].ships);
	free(p);
	for(i = 0 ; i < 6 ;i ++)
	{
		free(c[i]);
	}
	free(c);
}
void init_main_win(WIN* win,int h,int w,int lines,int cols)
{
	win->height = h;//inaltimea ferestrei principale
	win->width = w;// latimea ferestrei principale
	win->starty = (lines - win->height)/2;	//coordonata x a punctului pivot
	win->startx = (cols - win->width)/2;// coordonata y a punctului pivot
	win->border.ls = '#';// caracterele de pe marginea stanga
	win->border.rs = '#';// si de pe marginea dreapta
	win->border.ts = '*';// caracterele de sus
	win->border.bs = '*';// si de jos 
	win->window = newwin(win->height,win->width,win->starty+1,win->startx);
}
void mainScreen(WIN* win,char** ascii_art)
{
	//printez ascii-art-ul 
	//mut cursorul in asa fel incat jocul sa fie centrat in fereastra
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
	{	if(highlight == i + 1) // evidentiez optiunea prin highlight
		{	
			wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}
//creez o noua fereastra de inaltime heighy si grosime width
WINDOW* create_newwin(int height,int width,int starty,int startx) 
{
	WINDOW* local_win;
	local_win = newwin(height,width,starty,startx);
	wrefresh(local_win);
	return local_win;
}
WINDOW* initNewGame(WIN* main_win, char* my_board_text)//modificat
{
	//creez fereastra de joc	
	WINDOW* win = create_newwin(31,110,main_win->starty+1,main_win->startx);
	//printez tabla mea
	char** my_board;
	my_board = file_in_char(my_board_text);//modificat
	int y,x,i,j;
	y = 10;
	x = 10;
	for(i = 0; i < 10; ++i)
	{		
		mvwprintw(win, y, x, "%s", my_board[i]);
		++y;
	}
	//printez tabla inamica goala
	char** enemy_field;
	enemy_field = file_in_char("enemy_board.txt");
	getmaxyx(win,y,x);
	y = 10;
	for(i = 0; i < 10; ++i)
	{		
		mvwprintw(win, y, x-30, "%s", enemy_field[i]);
		++y;
	}
	for(i = 0 ; i < 10;i++)
	{
		free(my_board[i]);
		free(enemy_field[i]);
	}
	free(my_board);
	free(enemy_field[i]);
	return win;
}
void Game(WINDOW* win,WIN* main_win,PLAYER* player,int* count_visited) 
//CHANGES WITH PLAYER
//counts_hit nu se salveaza dupa ce ies din joc si dau resume
//trebuie verificata fiecare nava cu un for anchor pointsi merge pe 
//orienarea sa in functie de lungime
{
	int y,x,k;
	getmaxyx(win,y,x);
	keypad(win,TRUE);
	//move pe enemy_board
	int boardx = x-30,boardy = 10;
	int chr;
	boardx++;
	wmove(win,boardy,boardx);
	//for enter case
	int count_hits_me = 0;
	int count_hits_enemy = 0;
	int attacked = 0;
	//printez optiunile posibile
	wmove(win,28,1);
	wprintw(win,"PRESS Q TO EXIT");
	wmove(win,27,1);
	wprintw(win,"PRESS D TO DESTROY 10 SPACES EACH SIDE");	
	wmove(win,26,1);
	wprintw(win,"PRESS R TO REARANGE YOUR BOARD");
	wmove(win,boardy,boardx);
	int turn = 0;
	// in cazul in care se apasa D , tin minte cate casute vor fi distruse
	int bombs = 10;
	chr = 'a';
	while(chr != 'Q')
	{
		// daca e randul meu sa atac foloses cheile tastaturii sa ma misc
		// de-a lungul tablei de joc
		if(turn == 0)
		{
			chr = wgetch(win);
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
			if(boardy+1 <20)
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
			// cu enter selectez celula curenta
			case '\n':
			// daca casuta nu a fost vizitata si in celula respectiva se
			// afla o bucatica de nava atunci apare un X pe celula respectiva
			if(player[1].board[boardy - 10][(boardx-80)/2] == 1 &&
			   player[0].visited[boardy - 10][(boardx-80)/2] == 0)
			{
				
				wattron(win,COLOR_PAIR(1));
				wprintw(win,"X");
				wattroff(win,COLOR_PAIR(1));
				player[0].visited[boardy - 10][(boardx-80)/2] = 1;
				player[0].hit = player[0].hit + 1;
				// verific ce iterator are nava lovita
				k = which_boat(boardy-10,(boardx-80)/2,player,1);
				player[1].ships[k].damaged++;
				attacked = 1;
				turn = 0;
			}
			// daca nu e vizitata celula si nu e nimic in celula respectiva
			// se afiseaza o "-"
			else if(player[1].board[boardy - 10][(boardx-80)/2] == 0 &&
			 		player[0].visited[boardy - 10][(boardx-80)/2] == 0)
			{
				
				wattron(win,COLOR_PAIR(2));
				wprintw(win,"-");
				wattroff(win,COLOR_PAIR(2));
				player[0].visited[boardy - 10][(boardx-80)/2] = 1;
				attacked = 1;
				turn = 1;
			}
			break;
			//distruge 10 nave in fiecare parte
			case 'D':
			bombs = 10;
			while(bombs != 0 && *count_visited<100)
			{
				// aleg 10 celule nealese pe care sa le incerce in
				// fiecare parte
				int y_rand = rand() % 10;
				int x_rand = rand() % 10;
				// caut un y si un x pana cand gasesc o celula nevizitata
				while(player[1].visited[y_rand][x_rand] == 1)
				{
					y_rand = rand() % 10;
					x_rand = rand() % 10;
				}
				player[1].visited[y_rand][x_rand] = 1;
				// daca nimereste o nava inamica afiseaza #
				if(player[0].board[y_rand][x_rand] == 1)
					{
						wmove(win,y_rand+10,x_rand*2+11);
						wattron(win,COLOR_PAIR(1));
						waddch(win,'#');
						wattroff(win,COLOR_PAIR(1));
						player[1].hit++;//schimba
						int k = which_boat(y_rand,x_rand,player,0);
						player[0].ships[k].damaged++;
						wmove(win,boardy,boardx);
				}
				// daca nu nimereste printeaza "-"
				else
				{
					wmove(win,y_rand+10,x_rand*2+11);
					wattron(win,COLOR_PAIR(2));
					wprintw(win,"-");
					wattroff(win,COLOR_PAIR(2));
					wmove(win,boardy,boardx);
				}
				//configurat pentru a ataca partea inamica
				y_rand = rand() % 10;
				x_rand = rand() % 10;
				// caut un y si un x pana cand gasesc o celula nevizitata
				while(player[0].visited[y_rand][x_rand] == 1)
				{
					y_rand = rand() % 10;
					x_rand = rand() % 10;
				}
				// daca nimeresc o bucata de nava se marcheaza cu un X
				player[0].visited[y_rand][x_rand] = 1;
				if(player[1].board[y_rand][x_rand] == 1)
				{
						wmove(win,y_rand+10,x - 30 + (x_rand)*2 +1);
						wattron(win,COLOR_PAIR(1));
						wprintw(win,"X");
						wattroff(win,COLOR_PAIR(1));
						player[0].hit++;
						k = which_boat(y_rand,x_rand,player,1);
						player[1].ships[k].damaged++;
						wmove(win,boardy,boardx);
				}
				// daca nu nimeresc se noteaza cu o liniuta
				else
				{
					wmove(win,y_rand+ 10 ,x - 30 + (x_rand)*2+1);
					wattron(win,COLOR_PAIR(2));
					wprintw(win,"-");
					wattroff(win,COLOR_PAIR(2));	
					wmove(win,boardy,boardx);
				}
				wrefresh(win);
				bombs--;
				*count_visited = *count_visited + 1;
				// se numara vizitele pentru a nu depasi numarul  maxim de 
				// incercari
			}
			break;
			case 'R':
			// reinitializez numarul de lovituri 	
			player[1].hit = 0;
			//reinitializez matricea vizitata
			player[1].visited = init_mat();
			//rearanjez tabgla
			rearange_board(&player[0]);
			from_matrix_to_board(player[0].board,win);
			wmove(win,boardy,boardx);
			wrefresh(win);
			break;
		}
		}
		wrefresh(win);
		raw();
		//computer is attacking
		while(turn == 1 && *count_visited < 100)
		if(attacked == 1)
		{
			//daca am atacat calculatorul, asteapta doua secunde, dupa care
			// se ia in considerare o alta miscare
			sleep(2);
			// /continue;
			//!!!numar 3 secunde in care se va aplica getch()!!!!
			attacked = 0;
			int y_rand = rand() % 10;
			int x_rand = rand() % 10;
			while(player[1].visited[y_rand][x_rand] == 1 )
			{
				y_rand = rand() % 10;
				x_rand = rand() % 10;
			}
			player[1].visited[y_rand][x_rand] = 1;
			if(player[0].board[y_rand][x_rand] == 1)
			{
				wmove(win,y_rand+10,x_rand*2+11);
				wattron(win,COLOR_PAIR(1));
				waddch(win,'#');
				wattroff(win,COLOR_PAIR(1));
				player[1].hit++;//daca inamicul a lovit, 
				//creste numarul de lovituri 
				k = which_boat(y_rand,x_rand,player,0);
				player[0].ships[k].damaged++;//ce barca a fost lovita
				wmove(win,boardy,boardx);
				turn = 1;//setez ca tura sa fie a jucatorului
				attacked = 1;//retin ca inamicul a atacat
				//aceasta variabila e folosita pentru a permite
				//jucatorului (PC sau uman) sa poata ataca iar
				//in cazul in care loveste o bucata de nava
			}
			else
			{
				wmove(win,y_rand+10,x_rand*2+11);
				wattron(win,COLOR_PAIR(2));
				wprintw(win,"-");
				wattroff(win,COLOR_PAIR(2));
				wmove(win,boardy,boardx);
				turn = 0;
			}
			*count_visited = *count_visited + 1;
			// pentru a nu sari de numarul de spatii
			wrefresh(win);
		}
		wrefresh(win);	
		// daca e egalitate, jocul printeaza in mijloc sus mesajul DRAW !
		if(player[0].hit == 20 && player[1].hit == 20) 
		{
			wmove(win,5,boardx/2 + 5);
			wprintw(win,"DRAW !");
		
		}
		// daca ai pierdut apare esajul YOU LSOT !
		else if(player[1].hit == 20)
		{
			wmove(win,5,boardx/2 + 5);
			wprintw(win,"YOU LOST !");
		}
		// daca ai castigat apare mesajul YOU WON !
		else if(player[0].hit == 20)
		{
			wmove(win,5,boardx/2 + 5);
			wprintw(win,"YOU WON !");
		}
		//salvez intr-un savefile starea jocului in care ma aflu 
		// pentru cazul in care jucatorul vrea sa iasa si sa 
		// foloseasca optiunea RESUME GAME
		scr_dump("savefile.txt");
		int index;
		if(player[1].hit == 20 || player[0].hit == 20
			|| *count_visited >= 100)
		{
		// iau fiecare nava la rnad, din fiecare tabla, si verific cate 
		// parti din ele au fost ditruse.
		// daca partile distruse sunt in numar egale cu lungimea lor 
		// atunci navele se considera ca fiind distruse complet
		for(index = 0; index < 10;index++)
		{
			if(player[0].ships[index].damaged == player[0].ships[index].len)
			{
				player[1].count_destroyed++;
			}
			
			if(player[1].ships[index].damaged == player[1].ships[index].len)
			{
				player[0].count_destroyed++;
			}
		}
		// sub starea finala a jocului se afiseaza numarul de nave distruse
		// de fiecare dintre jucatori
		wmove(win,6,boardx/2 + 5);
		wprintw(win,"THE ENEMY DESTROYED %d SHIPS",player[1].count_destroyed);
		wmove(win,7,boardx/2 + 5);
		wprintw(win,"YOU DESTROYED %d SHIPS",player[0].count_destroyed);
		wrefresh(win);
		raw();
		//dupa terminarea jocurlui , acesta devine inactiv 10 secunde
		sleep(10);
		//continue;
		break;
		}
	}
}
void rearange_board(PLAYER* player)
{
	int i;
	SHIPS* nava;
	int nr = 0;
	int** matrix = init_mat();
	nava = malloc(10 * sizeof(SHIPS));//aloc memorie pentru nava
	for( i = 0 ; i < 10 ; i++)
	{
		//iau fiecare nava si verific daca a fost distrusa
		//daca nu a fost distrusa
		if(player->ships[i].damaged != player->ships[i].len)
		{
			//resetez lungimea
			player->ships[i].len -= player->ships[i].damaged;
			//resetez cate casute avea lovite
			player->ships[i].damaged = 0;
			nava[nr] = player->ships[i];
			//salvez navele care nu sunt complet distruse
			nr++;
		}
	}
	//creez tabla in functie de navele noi formate
	player->board = generate_board_by_struct(nava,nr);
	nr = 0;
	for( i = 0 ; i < 10; i ++)
	{
		//dau navelor vechi valoarea navelor noi
		if(player->ships[i].damaged != player->ships[i].len)
		{
			player->ships[i] = nava[nr]; 
			nr++;
		}
	}
	free(nava);
}
int which_boat(int y,int x,PLAYER* player,int id_player)
{
	//aflu punctul pivot
	int found = 0;
	while(found == 0)
	{
		// ma deplasez pe orizontala sau pe veritcala pana gasesc
		//punctul pivot
		if(x - 1 >=0 && player[id_player].board[y][x-1] == 1)
			x = x - 1;
		else if(y - 1 >= 0 && player[id_player].board[y-1][x] == 1)
			y = y -1;
		else
			found = 1;
	}	
	//verific carei nave apartine
	int i;
	//refac coordonatele punctului pivot pentru fiecare nava
	for(i = 0 ; i < 10; i ++)
	{
		if(player[id_player].ships[i].startx == x && 
			player[id_player].ships[i].starty == y)
		{
			
			return i;
		}
	}
}
int** init_mat()
{
	int** matrice;
	int i;
	matrice = calloc(12,sizeof(int*));
	for(i = 0; i < 12; i ++)
		matrice[i] = calloc(12,sizeof(int));
	return matrice;
}
int verificare_OR(int y,int x,int len,int** interzis)
{
	int i,j;
	int ok = 1;
	//interzis reprezinta faptul ca nu am voie sa pun o nava acolo
	//el se actualizeaza pe masura ce sa aseaza navele
	//verific daca nava mea va intra pe o casuta interzisa
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

int nav_gen_OR(int** matrice,int** interzis,int len,SHIPS* nave)
{
	int i,j;
	//lucrez pe o matrice de la 1 la 10
	//se genereaza doua coordonate aleatoare care reprezinta pivotul navei
	int diry = rand()%10 + 1;
	int dirx = rand()%(10 - len - 1) + 1;
	int ok; 
	// verific daca pozitia e valabila
	ok = verificare_OR(diry,dirx,len,interzis);
	if(ok == 1)
	{
		nave->starty = diry-1;
		nave->startx = dirx-1;
		//pentru fiecare celula de-a lungul lungimii navei 
		// se vor construi :
		for( j = dirx; j < dirx + len ;j++ )
		{
			// nava
			matrice[diry][j] = 1;
			//zonele care devin inaccesibile altor nave
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
	//se returneaza daca nava a putu sau nu fi pusa in locul aleatoru
	return ok;
}
// analog ca mai sus, doar ca pe verticala
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
int nav_gen_VR(int** matrice,int** interzis,int len,SHIPS* nave)
{
	int i,j;
	int dirx = rand()%10 + 1;
	int diry = rand()%(10 - len - 1) + 1;//(10 - len - 1)
	int ok = verificare_VR(diry,dirx,len,interzis);
	if(ok == 1)
	{
		nave->starty = diry-1;
		nave->startx = dirx-1;
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
//transform o matrice de la 1 la 10 sa fie de la 0 la 9
void normalize(int** matrix)
{
	int i,j;
	for(i = 1; i <= 10;i++)
	{
		for(j = 1 ; j <= 10; j ++)
		{
			matrix[i-1][j-1] = matrix[i][j];
			matrix[i][j] = 0;	
		}
	}
}
int** generate_enemy_board()
{
	int** matrice = init_mat();
	int** interzis = init_mat();
	//initial sunt 4 tipuri de nave
	SHIPS nave[4];
	//numar nave
	nave[0].nr = 1;
	nave[1].nr = 2;
	nave[2].nr = 3;
	nave[3].nr = 4;
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
	while(index_nave<4)
	{	
		//luam fiecare tip de nave la rand;
		dir_rand = rand()%2;
		if(dir_rand == 1)//orizontal
		{
		verif_if_placed = nav_gen_OR(matrice,interzis,nave[index_nave].len,
									nave);
		}
		else//vertical
		{
		verif_if_placed = nav_gen_VR(matrice,interzis,nave[index_nave].len,
									nave);
		}
		if(verif_if_placed == 1)
			nave[index_nave].nr --;
		
		if(nave[index_nave].nr == 0)
		index_nave++;
	}
	normalize(matrice);
	return matrice;
}
int** generate_board_by_struct(SHIPS* nave,int nr_nave)
{
	//de adaugat tot ce trebuie
	int** matrice = init_mat();
	int** interzis = init_mat();

	int index_nave = 0;
	int i,j;
	srand(time(0));
	int dir_rand;
	int verif_if_placed;
	while(index_nave < nr_nave)
	{	
		//luam fiecare tip de nava la rand;
		dir_rand = rand()%2;
		if(dir_rand == 1)//orizontal
		{
			nave[index_nave].direction = 0;
		verif_if_placed = nav_gen_OR(matrice,interzis,nave[index_nave].len
									,&nave[index_nave]);
		}
		else//vertical
		{
			nave[index_nave].direction = 1;
		verif_if_placed = nav_gen_VR(matrice,interzis,nave[index_nave].len
									,&nave[index_nave]);
		}
		if(verif_if_placed == 1) // daca pozitia era valabila
		index_nave++;
	}
	normalize(matrice);//transofrm intr-o matrice de la 0 la 9
	return matrice;
}
void from_matrix_to_board(int** matrix,WINDOW* win)
{
	int y,x,i,j;
	y = 10;
	x = 10;
	for(i = 0; i < 10; ++i)
	{		
		x = 10;
		for(j = 0 ; j <= 20;j++)
		{
			if(j % 2 == 0)
			{
				mvwprintw(win,y,x,"|");
			}
			else if(matrix[i][j/2] == 1)
			{
				wmove(win,y,x);
				waddch(win,'X');
			}
			else if(matrix[i][j/2] == 0)
			{
				mvwprintw(win, y, x, " ");

			}
			++x;
		}
		++y;
	}
}
//pentru tabla mea
int** my_board_in_01(char*  my_board_text)//modificat
{
	char** string;
	int** my_matrix = init_mat();
	int i,j;

	//transform text caractere in matrice 0 si 1
	string = file_in_char(my_board_text);//modificat

	for(i = 0 ; i < 10 ; i ++)
	{
		for( j = 0 ; j < 20; j ++)
		{
			if(string[i][j] == 'X')
			{
				my_matrix[i][j/2] = 1;
			}
		}
	}
	for(i = 0 ; i < 10;i ++)
	{
		free(string[i]);
	}
	free(string);
	return my_matrix;
}
void create_box(WIN *p_win) //din documentatie
{
	int i, j;
	int x, y, w, h;

	x = p_win->startx;
	y = p_win->starty;
	w = p_win->width;
	h = p_win->height;
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
//transform o matrice in entitati
SHIPS* in_entities(int** matrix)
{
	int i,j;
	SHIPS* nave;
	nave = malloc(20*sizeof(nave));
	int nr_nave = 0;
	int** visited;
	visited = init_mat();
	for( i = 0; i < 10; i ++)
	{
		for(j = 0; j < 10; j ++)
		{
			// incep cautarea de la stanga la dreapta si de sus in jos
			// daca gasesc un "1" care nu a facut pana atunci parte 
			//dintr-o nava, este considerat punctul pivot al navei
			if(matrix[i][j] == 1 && visited[i][j] == 0)
			{
				//retin coordonatele punctului pivot
				nave[nr_nave].starty = i;
				nave[nr_nave].startx = j;
				//initializez lungimea acestuia
				nave[nr_nave].len = 1;
				//initializez de cate ori a fost lovit
				nave[nr_nave].damaged = 0;
				//si notez celula respectiva ca fiind vizitata
				//pentru a evita eventuale erori de suprapunere
				visited[i][j] = 1;
				nave[nr_nave].direction = -1;
				//daca nava e orizontala
				if(j+1 < 10 && matrix[i][j+1] == 1)
				{
					nave[nr_nave].direction = 0;
					//pentru a vedea lungimea navei
					int jaux = j+1;
					while(jaux < 10 && matrix[i][jaux] == 1)
					{
						//blochez posibila prezenta a altor nave
						visited[i][jaux] = 1;
						//cresc lungimea navelor
						nave[nr_nave].len = nave[nr_nave].len + 1;
						//avansez
						jaux++;
					}
				}
				else if(i+1 < 10 && matrix[i+1][j] == 1)
				{
					nave[nr_nave].direction = 1;
					int iaux = i+1;
					while(iaux < 10 && matrix[iaux][j] == 1)
					{
						visited[iaux][j] = 1;
						nave[nr_nave].len = nave[nr_nave].len + 1;
						iaux++;
					}
				}
				nr_nave++;
			}
		}
	}
	return nave;
}