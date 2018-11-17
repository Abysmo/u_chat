#include "terminal_ui.h"


//char win_buffer[1024]={0};
chtype i_char = 0;
int w_rows, w_cols, cur_posX, cur_posY;


struct message 
{
	char text[MSG_MAXLEN];
	char * cursor;
	int cursor_pos;
}ms;

void init_struct()
{
	ms.cursor = ms.text;
	memset(ms.text,'\0',MSG_MAXLEN);
	ms.cursor_pos = 0;
}

	
WINDOW * create_msgbox_win()
{	
	getmaxyx(stdscr, w_rows, w_cols);

	WINDOW * win = newwin(w_rows-1, w_cols, 0, 0);
	//wborder(win,ACS_VLINE,ACS_VLINE,ACS_BOARD,ACS_BOARD,ACS_BOARD,ACS_BOARD,ACS_BOARD,ACS_BOARD);
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_CYAN);
	wcolor_set(win, 1, NULL);
	wbkgd(win,COLOR_PAIR(1));
	scrollok(win, TRUE);
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
	nodelay(win, TRUE);
	keypad(win, TRUE);
	//wprintw(win,"TEST_SEND\n"); //test
	wrefresh(win);
	return win;
}


void ncurses_setup()
{
	if(!initscr())
	{
		fprintf(stderr,"Ncurses win init fail");
		exit(1);
	}
	refresh();

	cbreak();//<<--
	//nonl();
	//timeout(0);//<<--
	//leaveok(stdscr,TRUE);//<<--
	//raw();
	nodelay(stdscr, TRUE);//<<--
	//curs_set(0);
	noecho();//<<--
	keypad(stdscr, TRUE);//<<--
	init_struct();
	
}


char * send_msg_handler(WINDOW * sendwin)
{
	if((i_char = getch())==ERR) return ms.text;
	else if (i_char == KEY_BACKSPACE) 
	{
		getyx(sendwin, cur_posY, cur_posX);
		wmove(sendwin,	cur_posY, cur_posX-1);		
		wdelch(sendwin);
		wrefresh(sendwin);
	}
	else if (i_char == KEY_DC) 
	{	
		wdelch(sendwin);
		wrefresh(sendwin);
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
	else if (i_char == '\n') 
	{
		*ms.cursor = '\n';
		ms.cursor++;		
		*ms.cursor = '\0';
		ms.cursor = ms.text;
		ms.cursor_pos = 0;
		wclear(sendwin);
		wrefresh(sendwin);
		return ms.text;
	}
	else
	{	
				
		wechochar(sendwin, i_char);
		*ms.cursor = (char)i_char;
		ms.cursor++;
		return ms.text;
	}
return ms.text;
}





