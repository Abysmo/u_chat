#ifndef _TERMINAL_UI_H
#define _TERMINAL_UI_H

#include <ncurses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_MAXLEN 1024

WINDOW * create_msgbox_win();
WINDOW * create_msgsend_win();
void ncurses_setup();
char * send_msg_handler(WINDOW * sendwin);

#endif
