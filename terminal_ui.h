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

#define IN_BOX          w1
#define OUT_BOX     w2
#define USR_BOX     w3

#define ASCII_MAX 127

//#define FORBIDDEN_KEYS

WINDOW * create_msgbox_win();
WINDOW * create_msgsend_win();
WINDOW * create_usrbox_win();
int is_ascii(char * x);
void ncurses_setup();
char * key_handler(WINDOW * sendwin);

WINDOW * IN_BOX;
WINDOW * OUT_BOX;
WINDOW * USR_BOX;
long i_char;
int w_rows, w_cols, cur_posX, cur_posY;

#endif
