#include "terminal_ui.h"
#include "users.h"

//initilize input string
void init_text()
{
    memset(text_buff,'\0',MSG_MAXLEN);
    text_cursor = 0;
    wclear(OUT_BOX);
    wrefresh(OUT_BOX);
}
	
WINDOW * create_msgbox_win()
{	
	getmaxyx(stdscr, w_rows, w_cols);

    WINDOW * win = newwin(w_rows-3, w_cols-NAME_LEN, 0, NAME_LEN);
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

    WINDOW * win = newwin(w_rows-3, 16, 0, 0);
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

    //WINDOW * win = newwin(1, w_cols, w_rows-1, 0);
    //WINDOW * win = newwin(3, MSG_MAXLEN, w_rows-3, 0);
    WINDOW * win = newwin(3, w_cols, w_rows-3, 0);
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

//performing ncurses windows parameters and turn on Ncurses mode
void ncurses_setup()
{
	if(!initscr())
	{
		fprintf(stderr,"Ncurses win init fail");
		exit(1);
	}
	refresh();

    cbreak();//<<--raw mode with signals
    nodelay(stdscr, TRUE);//<<--nonblock input
    noecho();//<<-- no echo for real cursor
    keypad(stdscr, TRUE);//<<-- keypad mode
    init_text();
}

//draw windows
void draw_UI()
{
    endwin();
    refresh();

    USR_BOX =  create_usrbox_win();
    OUT_BOX = create_msgsend_win();
    IN_BOX = create_msgbox_win();
}

//check char is ascii or not
int is_ascii(char * x)
{

    unsigned int z = *x;
    if (z > ASCII_MAX) return 0;
    else return 1;
}

//count how much visible chars in string(multibyte chars counts as one)
int multichar_in_str(char * string)
{
    char * s_cur = string;
    int chrcount = 0;
    for (;*s_cur != '\0';)
    {
        if (is_ascii(s_cur))
        {
            chrcount++;
            s_cur++;
        }
        else
        {
            chrcount ++;
            s_cur +=2;
        }
    }
    return chrcount;
}

//finding beginning of string in input window when cursor is moving on previous line
char * find_str_begin(WINDOW * win, char string[MSG_MAXLEN], int cursor_pos)
{
    int counter = win->_maxx;
    char * s_beg = &string[cursor_pos];
    while ((counter > 0) && (s_beg != string))
    {
        if (is_ascii(s_beg))
        {
            s_beg--;
            counter--;
        }
        else
        {
            s_beg-=2;
            counter--;
        }
    }
    return s_beg;
}

//main key handling function
char * key_handler(WINDOW * sendwin)
{
    getyx(sendwin, cur_posY, cur_posX);
    wmove(sendwin,	cur_posY, cur_posX);
    wrefresh(sendwin);
    extern int sock, sock_recv;

    if((i_char = getch())==ERR) return text_buff;
    else if (i_char == KEY_RESIZE)
    {
        draw_UI();
        init_text();
    }
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
        if (is_ascii(&text_buff[text_cursor-1]))
        {
            strcpy(temp_buff, &text_buff[text_cursor]);
            memset(&text_buff[text_cursor-1], '\0', strlen(temp_buff)+1);
            text_cursor--;
            strncpy(&text_buff[text_cursor],temp_buff,strlen(temp_buff));
        }
        else
        {
            strcpy(temp_buff, &text_buff[text_cursor]);
            memset(&text_buff[text_cursor-2], '\0', strlen(temp_buff)+2);
            text_cursor-=2;
            strncpy(&text_buff[text_cursor],temp_buff,strlen(temp_buff));
        }

        if (!cur_posX && !cur_posY)
        {
            wclear(sendwin);
            scrollok(sendwin,FALSE);
            waddstr(sendwin, find_str_begin(sendwin,text_buff,text_cursor));
            wmove(sendwin, 0, sendwin->_maxx);
            wdelch(sendwin);
            wrefresh(sendwin);
            scrollok(sendwin,TRUE);
            return text_buff;
        }
        else if(!cur_posX && cur_posY)
        {
            wmove(sendwin, --cur_posY, sendwin->_maxx);
            wdelch(sendwin);
            wrefresh(sendwin);
            return text_buff;
        }
        else
        {
            wmove(sendwin, cur_posY, --cur_posX);
            wdelch(sendwin);
            wrefresh(sendwin);
            return text_buff;
        }
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

        if (!cur_posX && !cur_posY)
        {
            wclear(sendwin);
            scrollok(sendwin,FALSE);
            waddstr(sendwin, find_str_begin(sendwin,text_buff,text_cursor));
            wmove(sendwin, 0, sendwin->_maxx);
            wrefresh(sendwin);
            scrollok(sendwin,TRUE);
            return text_buff;
        }
        else if(!cur_posX && cur_posY)
        {
            wmove(sendwin, --cur_posY, sendwin->_maxx);
            wrefresh(sendwin);
            return text_buff;
        }
        else
        {
            wmove(sendwin, cur_posY, --cur_posX);
            wrefresh(sendwin);
            return text_buff;
        }

	}
	else if (i_char == KEY_RIGHT)
	{
        if (text_cursor >= (MSG_MAXLEN-3) || (text_buff[text_cursor] =='\0'))return text_buff;
        if (is_ascii(&text_buff[text_cursor]) && is_ascii(&text_buff[text_cursor+1]))
        text_cursor++;
        else if (is_ascii(&text_buff[text_cursor]) && (!is_ascii(&text_buff[text_cursor+1])))
        text_cursor++;
        else text_cursor +=2;

        if(cur_posX ==  sendwin->_maxx)
            wmove(sendwin,	++cur_posY, 0);
        else
            wmove(sendwin, cur_posY, ++cur_posX);
        wrefresh(sendwin);
	}
    else if (i_char == '\n') //KEY ENTER
	{
        text_buff[strlen(text_buff)] = '\n';
        return text_buff;
	}
    else //putchar in buffer
    {
        char temp_buff[MSG_MAXLEN] = {0};
        if (text_buff[text_cursor] != '\0') //putchar with shifting (inserting char)
        {
            if (i_char < ASCII_MAX)
            {
                strncpy(temp_buff, &text_buff[text_cursor], strlen(&text_buff[text_cursor]));
                text_buff[text_cursor] = (char)i_char;
                strncpy(&text_buff[++text_cursor],temp_buff,strlen(temp_buff));
                winsch(sendwin,i_char);
                if(cur_posX ==  sendwin->_maxx)
                    wmove(sendwin,	++cur_posY, 0);
                else
                    wmove(sendwin, cur_posY, ++cur_posX);
                wrefresh(sendwin);
                return text_buff;
            }
            else
            {
                strncpy(temp_buff, &text_buff[text_cursor], strlen(&text_buff[text_cursor]));
                text_buff[text_cursor] = (char)i_char;
                winsch(sendwin,i_char);
                i_char = getch();
                text_buff[text_cursor+1] = (char)i_char;
                strncpy(&text_buff[text_cursor+2],temp_buff,strlen(temp_buff));
                text_cursor+=2;
                winsch(sendwin,i_char);
                if(cur_posX ==  sendwin->_maxx)
                    wmove(sendwin,	++cur_posY, 0);
                else
                    wmove(sendwin, cur_posY, ++cur_posX);
                wrefresh(sendwin);
                return text_buff;
            }
        }
        else
        {
            if (i_char < ASCII_MAX)
            {
                text_buff[text_cursor] = (char)i_char;
                text_cursor++;
                wechochar(sendwin, i_char);
                return text_buff;
            }
            else
            {
                text_buff[text_cursor] = (char)i_char;
                wechochar(sendwin, i_char);
                i_char = getch();
                text_buff[text_cursor+1] = (char)i_char;
                text_cursor+=2;
                wechochar(sendwin, i_char);
                return text_buff;
            }
        }
    }
return text_buff;
}





