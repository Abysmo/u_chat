#ifndef _TERMINAL_UI_H
#define _TERMINAL_UI_H

#include <ncurses.h>

WINDOW * create_msgbox_win();
WINDOW * create_msgsend_win();
void ncurses_setup();
char * send_msg_handler(WINDOW * sendwin);

#endif
