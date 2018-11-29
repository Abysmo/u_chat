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
#include <time.h>

//#define ADDR "192.168.0.255" // for inet_addr()
//#define ADDR(A,B,C,D) ((A<<24) | (B << 16) | (C << 8) | (D)) // for htonl()

#ifndef MSG_MAXLEN
#define MSG_MAXLEN 1024
#endif


#define PORT 5050
#define SRVC_CMD_SEP 0x1e //separator for dividing text and service messages

void name_broadcast(char * name);
void refresh_list(WINDOW * list_win, net_users_t * root);

char * msg_ptr=NULL;
char name_msg[NAME_LEN+1];
int sock;
struct sockaddr_in addr;

int main()
{
	setlocale(0, ""); //for unicode
	unsigned int income_addr_len;
    struct sockaddr_in local_addr, income_addr;
	char buf[MSG_MAXLEN]={0};
	int bytes_read;

    /*===============output_sock=================*/
    sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP); //UDP (SOCK_NONBLOCK)
    if(sock < 0)
    {
        fprintf(stderr,"Socket [addr] error : %s \n", strerror(errno));
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
	
    const char * bc_ip = getmyip(0,1);
    const char * local_ip = getmyip(1,0);
	if (bc_ip == NULL) {fprintf(stderr,"Failed to set IP ! \n"); exit(1);}
	addr.sin_addr.s_addr = inet_addr(bc_ip); 
    /*===========================================*/

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
	//bradcast premission
	unsigned int broadcastPermission = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
    fprintf(stderr,"Set Socket option error : %s \n", strerror(errno));
    /*===========================================*/


    /* local user init */
    char local_name[NAME_LEN]={0};
    printf("Set your nickname (up to 15 chars) : \n");
    fgets(local_name,NAME_LEN-1,stdin);
    //read(STDIN_FILENO,local_name,NAME_LEN-1);
    net_users_t * root = list_init(local_name, local_ip);

    /*making name for broadcast*/
    *name_msg = SRVC_CMD_SEP;
    strcat(name_msg, local_name);


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

    /*~~~~~~~~TEST FIELDS~~~~~~
    //struct net_user_list * next_user = add_user (root, "KEKS", "192.168.255.255");
    //struct net_user_list * super_user = add_user (root, "SUPER", "192.168.255.255");
    time_t tim_tmp = time(NULL);
    wprintw(win1,"%d\n", tim_tmp);
    wrefresh(win1);

    sleep(1);
    tim_tmp = time(NULL);
    wprintw(win1,"%d \n", tim_tmp);
    wrefresh(win1);

    ~~~~~~~~TEST FIELDS~~END~*/

    while (1)
	{	
        usleep(10000);
        name_broadcast(name_msg);
        refresh_list(win3, root);
        delete_timeout_users(root);
        msg_ptr = key_handler(win2);
		
        if (i_char == '\n' )
		{
            if(*msg_ptr == '\n') {*msg_ptr  = '\0'; continue;} // do not send blank string
            sendto(sock, msg_ptr, MSG_MAXLEN, 0,(struct sockaddr *)&addr, sizeof(addr));
            memset(msg_ptr, '\0', MSG_MAXLEN);
		}


        bytes_read = recvfrom(sock_recv, buf, MSG_MAXLEN, MSG_DONTWAIT, (struct sockaddr *)&income_addr, &income_addr_len);

        //if((bytes_read <= 0) || (bytes_read == EAGAIN)) {wprintw(win1,"error - %s", strerror(errno)); wrefresh(win1); continue;}
        if(bytes_read <= 0) {continue;}
        else
		{
			//if (strstr(inet_ntoa(income_addr.sin_addr), my_ip) != NULL){continue;} //if its my msg -> skip

            if (*buf == SRVC_CMD_SEP)
            {
                add_user (root, &buf[1], inet_ntoa(income_addr.sin_addr)); //add user if it's service msg
                continue;
            }

            wprintw(win1,"[%s]>>> %s",inet_ntoa(income_addr.sin_addr),buf);
			wrefresh(win1);
			
			getyx(win2, cur_posY, cur_posX);
			wmove(win2,	cur_posY, cur_posX);
			wrefresh(win2);	
			continue;
		}

	}

}
		
void name_broadcast(char * name)
{
    static time_t before = 0;
    time_t difference = 0;
    if (!before)
    {
        before = time(NULL);
        sendto(sock, name, NAME_LEN, 0,(struct sockaddr *)&addr, sizeof(addr));
        return;
    }

    difference =  time(NULL) - before;
    if (difference >= USER_TIMEOUT_S)
    {
        sendto(sock, name, NAME_LEN, 0,(struct sockaddr *)&addr, sizeof(addr));
        before = time(NULL);
    }
}



void refresh_list(WINDOW * list_win, net_users_t * root)
{
    net_users_t * cursor = root;
    static time_t before = 0;
    time_t difference = 0;
    if (!before)
    {
        before = time(NULL);
        wclear(list_win);
        //print list here
        do
        {
            wprintw(list_win,"%s", cursor->name);
            cursor = cursor->next;
        }
        while (cursor != NULL);
        wrefresh(list_win);
        return;
    }

    //check timer
    difference = time(NULL) - before;
    if (difference >= USER_TIMEOUT_S)
    {
        before = time(NULL);
        wclear(list_win);
        //print list here
        do
        {
            wprintw(list_win,"%s", cursor->name);
            cursor = cursor->next;
        }
        while (cursor !=NULL);
        wrefresh(list_win);
        return;
    }
    return;
}

