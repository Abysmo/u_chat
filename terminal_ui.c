#include <ncurses.h>
#include "terminal_ui.h"
//#include <>

void win3_income_msg_init()
{
	//ncurses 
	if(!initscr())
	{printf("Ncurses win3 init fail");exit(1);}

	int w_row, w_col;
	start_color();
	init_pair(1,  COLOR_BLUE, COLOR_WHITE);
	getmaxyx(stdscr, w_row, w_col);
	WINDOW * win = newwin(w_row-1, w_col, 0, 0);
	wcolor_set(win, 1, NULL);
		//clear();
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
	WINDOW * win = newwin(w_row-1, w_col, 0, 0);
	wcolor_set(win, 1, NULL);
		//clear();
}

void win1_outcome_msg_init()
{
	//ncurses 
	if(!initscr())
	{printf("Ncurses win1 init fail");exit(1);}

	int w_row, w_col;
	start_color();
	init_pair(1,  COLOR_BLUE, COLOR_WHITE);
	getmaxyx(stdscr, w_row, w_col);
	WINDOW * win = newwin(w_row-1, w_col, 0, 0);
	wcolor_set(win, 1, NULL);
		//clear();
}
