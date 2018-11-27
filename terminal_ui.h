#ifndef _TERMINAL_UI_H
#define _TERMINAL_UI_H

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#ifndef MSG_MAXLEN
#define MSG_MAXLEN 1024
#endif


//#define FORBIDDEN_KEYS

WINDOW * create_msgbox_win();
WINDOW * create_msgsend_win();
WINDOW * create_usrbox_win();
int is_ascii(char * x);
void ncurses_setup();
char * key_handler(WINDOW * sendwin);


#endif
