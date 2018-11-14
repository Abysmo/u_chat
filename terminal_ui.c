#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


//#include <>



chtype i_char;
int w_rows, w_cols, cur_posX, cur_posY;
	
WINDOW * create_msgbox_win()
{	
	getmaxyx(stdscr, w_rows, w_cols);

	WINDOW * win = newwin(w_rows-1, w_cols, 0, 0);
	wborder(win,ACS_VLINE,ACS_VLINE,ACS_BOARD,ACS_BOARD,ACS_BOARD,ACS_BOARD,ACS_BOARD,ACS_BOARD);
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	wcolor_set(win, 1, NULL);
	wbkgd(win,COLOR_PAIR(1));	
	//wprintw(win,"TEST_BOX\n"); //win test
	wrefresh(win);		
	return win;
}

WINDOW * create_msgsend_win()
{	
	getmaxyx(stdscr, w_rows, w_cols);

	WINDOW * win = newwin(1, w_cols, w_rows-1, 0);
	start_color();
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	wcolor_set(win, 2, NULL);
	wbkgd(win,COLOR_PAIR(2));
	//wprintw(win,"TEST_SEND\n"); //test
	wrefresh(win);
	return win;
}


void ncurses_setup()
{
	if(!initscr())
	{fprintf(stderr,"Ncurses win init fail");exit(1);}
	refresh();

	//cbreak();
	//nodelay(stdscr, TRUE);
	//delwin(stdscr);
	//curs_set(0);
	noecho();
	keypad(stdscr, TRUE);

}


void send_msg_handler(WINDOW * sendwin)
{
	wmove(sendwin,0,0);
	wrefresh(sendwin);	

	do
	{	
		if((i_char = getch())==ERR) continue;
		else if (i_char == KEY_BACKSPACE) 
		{
			getyx(sendwin, cur_posY, cur_posX);
			wmove(sendwin,	cur_posY, cur_posX-1);		
			wdelch(sendwin);
			wrefresh(sendwin);
			continue;
		}
		else if (i_char == KEY_DC) 
		{	
			wdelch(sendwin);
			wrefresh(sendwin);
			continue;
		}
		else if (i_char == KEY_LEFT) 
		{
			getyx(sendwin, cur_posY, cur_posX);
			wmove(sendwin,	cur_posY, cur_posX-1);
			wrefresh(sendwin);
		}
		else if (i_char == KEY_RIGHT) 
		{
			getyx(sendwin, cur_posY, cur_posX);
			wmove(sendwin,	cur_posY, cur_posX+1);
			wrefresh(sendwin);
		}
		else
		wechochar(sendwin, i_char);
	}
	while (i_char != KEY_UP);
	

	//getch();	
    endwin();
	exit(0);

}

