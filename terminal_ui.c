#include <ncurses.h>
#include <stdlib.h>
//#include <>

WINDOW * win3;
WINDOW * win2;
WINDOW * win1;

void win1_income_msg_init()
{
	//ncurses 
	if(!initscr())
	{printf("Ncurses win3 init fail");exit(1);}
	
	
	int w_rows, w_cols;
	start_color();
	init_pair(1,  COLOR_BLUE, COLOR_WHITE);
	getmaxyx(stdscr,w_rows, w_cols);
	win1 = newwin(w_rows-3, w_cols, 0, 1);
	box(win1, 0, 0);
	wcolor_set(win1, 1, NULL);

	clear();
	wprintw(win1,"w_cols:%d|w_rows%d",w_cols,w_rows); //test
	wrefresh(win1);
	
}


void win2_users_init()
{
	//ncurses 
	if(!initscr())
	{printf("Ncurses win2 init fail");exit(1);}

	int w_row, w_col;
	start_color();
	init_pair(1,  COLOR_BLUE, COLOR_WHITE);
	getmaxyx(stdscr, w_row, w_col);
	win2 = newwin(w_row-2, w_col, 0, 0);
	box(win2, 0, 0);
	wcolor_set(win2, 1, NULL);
		//clear();
}

void win3_outcome_msg_init()
{
	//ncurses 
	if(!initscr())
	{printf("Ncurses win1 init fail");exit(1);}

	int w_rows, w_cols;
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	getmaxyx(stdscr,w_rows, w_cols);
	win3 = subwin(win1, w_rows, 1, 0, w_rows-1);
	box(win3, 0, 0);
	wcolor_set(win3, 1, NULL);
	
	clear();
	
	wmove(win3, 0,0);
	wprintw(win3,"TEST!!123sad!!"); //test
	wrefresh(win3);
	wrefresh(win1);



}




