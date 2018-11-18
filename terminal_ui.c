#include "terminal_ui.h"

//char win_buffer[1024]={0};
chtype i_char = 0;
int w_rows, w_cols, cur_posX, cur_posY;


char text_buff[MSG_MAXLEN]= {0};
int text_cursor = 0;


void init_text()
{
    memset(text_buff,'\0',MSG_MAXLEN);
    text_cursor = 0;
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
    init_text();
	
}

int is_ascii(char * x)
{
    unsigned int z = (int)*x;
    if (z > 127) return 0;
    else return 1;
}

char * send_msg_handler(WINDOW * sendwin)
{
    if((i_char = getch())==ERR) return text_buff;
	else if (i_char == KEY_BACKSPACE) 
	{
		getyx(sendwin, cur_posY, cur_posX);
		wmove(sendwin,	cur_posY, cur_posX-1);		
		wdelch(sendwin);
		wrefresh(sendwin);
	}
	else if (i_char == KEY_DC) 
	{	
        char temp_buff[MSG_MAXLEN] = {0};

        if (is_ascii(&text_buff[text_cursor]))
        {
            strncpy(temp_buff, &text_buff[text_cursor+1], strlen(&text_buff[text_cursor+1]));
            memset(&text_buff[text_cursor], '\0', strlen(temp_buff)+1);
            strncpy(&text_buff[text_cursor],temp_buff,strlen(temp_buff));
        }
        else
        {
            strncpy(temp_buff, &text_buff[text_cursor+2], strlen(&text_buff[text_cursor+2]));
            memset(&text_buff[text_cursor], '\0', strlen(temp_buff)+2);
            strncpy(&text_buff[text_cursor],temp_buff,strlen(temp_buff));
            //if (!(is_ascii(&text_buff[text_cursor])))text_cursor--;
        }
/*
        int temp_cursor = text_cursor;

        if (is_ascii(text_buff[text_cursor]))
        {
            for (;text_buff[text_cursor] != '\0';text_cursor++)
            {
                text_buff[text_cursor] = text_buff[text_cursor+1];
            }
        }
        else
        {

            for (;text_buff[text_cursor] != '\0';text_cursor++)
            {
                text_buff[text_cursor] = text_buff[text_cursor+2];
            }
        }
        text_cursor = temp_cursor;
*/
		wdelch(sendwin);
		wrefresh(sendwin);
	}
	else if (i_char == KEY_LEFT) 
	{
        if (text_cursor <= 0) return text_buff;
        if (is_ascii(&text_buff[text_cursor]) && is_ascii(&text_buff[text_cursor-1]))
            text_cursor--;
        else if ((!is_ascii(&text_buff[text_cursor])) && is_ascii(&text_buff[text_cursor-1]))
            text_cursor --;
        else text_cursor -=2;
        getyx(sendwin, cur_posY, cur_posX);
		wmove(sendwin,	cur_posY, cur_posX-1);
		wrefresh(sendwin);
	}
	else if (i_char == KEY_RIGHT)
	{
        if (text_cursor >= (MSG_MAXLEN-3) || (text_buff[text_cursor] =='\0'))return text_buff;
        if (is_ascii(&text_buff[text_cursor]) && is_ascii(&text_buff[text_cursor+1]))
        text_cursor++;
        //else if ((!is_ascii(&text_buff[text_cursor])) && is_ascii(&text_buff[text_cursor-1]))
        else text_cursor +=2;
        getyx(sendwin, cur_posY, cur_posX);
		wmove(sendwin,	cur_posY, cur_posX+1);
		wrefresh(sendwin);
	}
	else if (i_char == '\n') 
	{

        text_buff[strlen(text_buff)] = '\n';

        text_cursor = 0;
		wclear(sendwin);
		wrefresh(sendwin);
        return text_buff;
	}
	else
	{	
		wechochar(sendwin, i_char);
        text_buff[text_cursor] = (char)i_char;
        text_cursor++;
        return text_buff;
	}
return text_buff;
}





