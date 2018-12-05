#include "terminal_ui.h"
#include "users.h"


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

    WINDOW * win = newwin(w_rows-1, w_cols-NAME_LEN, 0, NAME_LEN);
	start_color();
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    wcolor_set(win, 3, NULL);
    wbkgd(win,COLOR_PAIR(3));
	scrollok(win, TRUE);
	wrefresh(win);		
	return win;
}

WINDOW * create_usrbox_win()
{
    getmaxyx(stdscr, w_rows, w_cols);

    WINDOW * win = newwin(w_rows-1, 15, 0, 0);
    //wborder(win,ACS_VLINE,ACS_VLINE,ACS_BOARD,ACS_BOARD,ACS_BOARD,ACS_BOARD,ACS_BOARD,ACS_BOARD);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_CYAN);
    wcolor_set(win, 1, NULL);
    wbkgd(win,COLOR_PAIR(1));
    scrollok(win, TRUE);
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

    scrollok(win, TRUE);

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
	//raw();
	nodelay(stdscr, TRUE);//<<--
    //curs_set(0); // invis cursor
    noecho();//<<-- no echo for real cursor
    keypad(stdscr, TRUE);//<<-- keypad mode
    init_text();
}

int is_ascii(char * x)
{
    unsigned int z = (int)*x;
    if (z > ASCII_MAX) return 0;
    else return 1;
}

char * key_handler(WINDOW * sendwin)
{

    getyx(sendwin, cur_posY, cur_posX);
    wmove(sendwin,	cur_posY, cur_posX);
    wrefresh(sendwin);
    extern int sock, sock_recv;

    if((i_char = getch())==ERR) return text_buff;

    else if (i_char == 27) /*exit*/
    {
        extern  net_users_t * root;
        close_list(root);
        close(sock);
        close(sock_recv);
        endwin();
        exit(0);
    }
    else if (i_char == KEY_UP) /*scroll UP*/
    {
        wscrl(IN_BOX, -1);
        wrefresh(IN_BOX);
    }
    else if (i_char == KEY_DOWN) /*scroll DOWN*/
    {
        wscrl(IN_BOX, 1);
        wrefresh(IN_BOX);
    }
    else if (i_char == KEY_BACKSPACE)
	{
        if (text_cursor < 1) return text_buff;

        char temp_buff[MSG_MAXLEN] = {0};
        if (is_ascii(&text_buff[text_cursor]))
        {
            strncpy(temp_buff, &text_buff[text_cursor], strlen(&text_buff[text_cursor]));
            memset(&text_buff[text_cursor-1], '\0', strlen(temp_buff)+1);
            text_cursor--;
            strncpy(&text_buff[text_cursor],temp_buff,strlen(temp_buff));
        }
        else
        {
            strncpy(temp_buff, &text_buff[text_cursor-2], strlen(&text_buff[text_cursor]));
            memset(&text_buff[text_cursor-2], '\0', strlen(temp_buff)+2);
            text_cursor-=2;
            strncpy(&text_buff[text_cursor],temp_buff,strlen(temp_buff));
        }

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
        }
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
        /*TEST
        wclear(sendwin);
        if (text_cursor > sendwin->_maxx)
        {
            waddstr(sendwin,&text_buff[text_cursor - sendwin->_maxx]);
            getyx(sendwin, cur_posY, cur_posX);
            wmove(sendwin,	cur_posY, cur_posX-1);
            wrefresh(sendwin);
            return text_buff;
        }
        else
        {
            waddstr(sendwin,text_buff);
            getyx(sendwin, cur_posY, cur_posX);
            wmove(sendwin,	cur_posY, cur_posX-1);
            wrefresh(sendwin);
            return text_buff;
        }
        END TEST*/
        wrefresh(sendwin);
	}
	else if (i_char == KEY_RIGHT)
	{
        if (text_cursor >= (MSG_MAXLEN-3) || (text_buff[text_cursor] =='\0'))return text_buff;
        if (is_ascii(&text_buff[text_cursor]) && is_ascii(&text_buff[text_cursor+1]))
        text_cursor++;
        else if (is_ascii(&text_buff[text_cursor]) && (!is_ascii(&text_buff[text_cursor+1])))
        text_cursor++;
        else text_cursor +=2;
        getyx(sendwin, cur_posY, cur_posX);
		wmove(sendwin,	cur_posY, cur_posX+1);
        /*TEST
        wclear(sendwin);
        if (text_cursor > sendwin->_maxx)
        {
            waddstr(sendwin,&text_buff[text_cursor - sendwin->_maxx]);
            getyx(sendwin, cur_posY, cur_posX);
            wmove(sendwin,	cur_posY, cur_posX+1);
            wrefresh(sendwin);
            return text_buff;
        }
        else
        {
            waddstr(sendwin,text_buff);
            getyx(sendwin, cur_posY, cur_posX);
            wmove(sendwin,	cur_posY, cur_posX+1);
            wrefresh(sendwin);
            return text_buff;
        }
       END TEST*/
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
    else //putchar in buffer
    {
        char temp_buff[MSG_MAXLEN] = {0};
        if (text_buff[text_cursor] != '\0') //putchar without shifting (inserting char)
        {
            if (i_char < ASCII_MAX)
            {
                strncpy(temp_buff, &text_buff[text_cursor], strlen(&text_buff[text_cursor]));
                text_buff[text_cursor] = (char)i_char;
                strncpy(&text_buff[++text_cursor],temp_buff,strlen(temp_buff));

                wclear(sendwin);
                waddstr(sendwin,text_buff);
                wmove(sendwin, cur_posY, ++cur_posX);
                wrefresh(sendwin);
                return text_buff;
            }
            else
            {
                strncpy(temp_buff, &text_buff[text_cursor], strlen(&text_buff[text_cursor]));
                text_buff[text_cursor] = (char)i_char;
                i_char = getch();
                text_buff[text_cursor+1] = (char)i_char;
                strncpy(&text_buff[text_cursor+2],temp_buff,strlen(temp_buff));
                text_cursor+=2;

                wclear(sendwin);
                waddstr(sendwin,text_buff);
                wmove(sendwin, cur_posY, ++cur_posX);
                wrefresh(sendwin);
                return text_buff;
            }
        }
        else
        {
            text_buff[text_cursor] = (char)i_char;
            text_cursor++;

            /*ORIG*/
            wclear(sendwin);
            waddstr(sendwin,text_buff);
            wrefresh(sendwin);
            return text_buff;
            /*ORIG END*/

            /*TEST
            wclear(sendwin);
            if (text_cursor > sendwin->_maxx)
            {
                waddstr(sendwin,&text_buff[text_cursor - sendwin->_maxx]);
                wrefresh(sendwin);
                return text_buff;
            }
            else
            {
                waddstr(sendwin,text_buff);
                wrefresh(sendwin);
                return text_buff;
            }
            TEST END*/
        }
    }
return text_buff;
}





