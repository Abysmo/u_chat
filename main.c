/*
sudo apt-get install libncursesw5-dev
sudo apt-get install ncurses-dev
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h> //exit
#include <stdio.h> //printf
#include <unistd.h> //close
#include <string.h>
#include <errno.h>
//#include <signal.h> // sig handle
#include "get_my_ip.h"
#include "users.h"
#include "terminal_ui.h"
#include <locale.h>

//#define ADDR "192.168.0.255" // for inet_addr()
//#define ADDR(A,B,C,D) ((A<<24) | (B << 16) | (C << 8) | (D)) // for htonl()

#ifndef MSG_MAXLEN
#define MSG_MAXLEN 1024
#endif

#define PORT 5050

char * msg_ptr=NULL;

int main()
{
	setlocale(0, ""); //for unicode
	unsigned int income_addr_len;
    struct sockaddr_in addr,local_addr, income_addr;
	char buf[MSG_MAXLEN]={0};
	int bytes_read;

    /*===============output_sock=================*/
    int sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP); //UDP (SOCK_NONBLOCK)
    if(sock < 0)
    {
        fprintf(stderr,"Socket [addr] error : %s \n", strerror(errno));
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
	
	const char * bc_ip = getmyip(0,0);
	if (bc_ip == NULL) {fprintf(stderr,"Failed to set IP ! \n"); exit(1);}
	addr.sin_addr.s_addr = inet_addr(bc_ip); 

    /*===============input_sock================*/
    int sock_recv = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP); //UDP (SOCK_NONBLOCK)
    if(sock_recv < 0)
    {
        fprintf(stderr,"Socket [local_addr] error : %s \n", strerror(errno));
        exit(1);
    }

    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(PORT);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock_recv, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        fprintf(stderr,"Binding Socket error : %s \n", strerror(errno));
        exit(1);
    }
    /*===========================================*/

	//bradcast premission
	unsigned int broadcastPermission = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
    fprintf(stderr,"Set Socket option error : %s \n", strerror(errno));

	//ncurses
	extern chtype i_char;
	extern int cur_posX, cur_posY;
	ncurses_setup();
	WINDOW * win1;	
	WINDOW * win2;
    WINDOW * win3;

    win3 =  create_usrbox_win();
    win2 = create_msgsend_win();
    win1 = create_msgbox_win();

    while (1)
	{	

		msg_ptr = send_msg_handler(win2);
		
        if (i_char == '\n' )
		{
            if(*msg_ptr == '\n') {*msg_ptr  = '\0'; continue;} // do not send blank string
            sendto(sock, msg_ptr, MSG_MAXLEN, 0,(struct sockaddr *)&addr, sizeof(addr));
            memset(msg_ptr, '\0', MSG_MAXLEN);
		}

        bytes_read = recvfrom(sock_recv, buf, MSG_MAXLEN, MSG_DONTWAIT, (struct sockaddr *)&income_addr, &income_addr_len);

        if((bytes_read <= 0) || (bytes_read == EAGAIN)) {continue;}
		else 
		{
			//if (strstr(inet_ntoa(income_addr.sin_addr), my_ip) != NULL){continue;} //if its my msg -> skip

            wprintw(win1,"[%s]>>> %s",inet_ntoa(income_addr.sin_addr),buf);
			wrefresh(win1);
			
			getyx(win2, cur_posY, cur_posX);
			wmove(win2,	cur_posY, cur_posX);
			wrefresh(win2);	
			continue;
		}

	}

}
		

		


