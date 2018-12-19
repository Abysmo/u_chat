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
#define MSG_MAXLEN 2048
#endif


#define PORT 5050
#define SRVC_CMD_SEP 0x1e //separator for dividing text and service messages

void name_broadcast(char * name, struct sockaddr_in * addr); //TEST
void refresh_list(WINDOW * list_win, net_users_t * root, int force_refresh);
char * remove_newline(char * str);

char * msg_ptr=NULL;
char name_msg[NAME_LEN+1];
int sock, sock_recv;
//struct sockaddr_in addr;
net_users_t * root;

int main()
{
	setlocale(0, ""); //for unicode
	unsigned int income_addr_len;
    struct sockaddr_in local_addr, income_addr, addr;
	char buf[MSG_MAXLEN]={0};
	int bytes_read;
    char * income_name;

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
    sock_recv = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP); //UDP (SOCK_NONBLOCK)
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
    root = list_init(local_name, local_ip);

    /*making name for broadcast*/
    *name_msg = SRVC_CMD_SEP;
    strcat(name_msg, local_name);

	//ncurses
	ncurses_setup();
    USR_BOX =  create_usrbox_win();
    OUT_BOX = create_msgsend_win();
    IN_BOX = create_msgbox_win();
    refresh_list(USR_BOX, root,1);

    /*~~~~~MAIN CYCLE~~~~~*/
    while (1)
	{	
        usleep(10000);
        refresh_list(USR_BOX, root, 0);
        name_broadcast(name_msg, &addr);
        delete_timeout_users(root);
        msg_ptr = key_handler(OUT_BOX);
		
        if (i_char == '\n' ) /*ENTER KEY PRESSED*/
		{
            if(*msg_ptr == '\n') {*msg_ptr  = '\0'; continue;} // do not send blank string
            sendto(sock, msg_ptr, MSG_MAXLEN, 0,(struct sockaddr *)&addr, sizeof(addr));
            memset(msg_ptr, '\0', MSG_MAXLEN);
		}

        bytes_read = recvfrom(sock_recv, buf, MSG_MAXLEN, MSG_DONTWAIT, (struct sockaddr *)&income_addr, &income_addr_len);
        if(bytes_read <= 0) {continue;}
        else
		{
			//if (strstr(inet_ntoa(income_addr.sin_addr), my_ip) != NULL){continue;} //if its my msg -> skip

            if (*buf == SRVC_CMD_SEP)
            {
                add_user(root, &buf[1], inet_ntoa(income_addr.sin_addr)); //add user if it's service msg
                refresh_list(USR_BOX, root, 1);
                continue;
            }

            if ((income_name = find_user(root, inet_ntoa(income_addr.sin_addr))) == NULL) {continue;}//do not accept messages from unregistred users

            wprintw(IN_BOX,"[%s]-> %s", remove_newline(income_name) , buf); //print message procedure
            wrefresh(IN_BOX);
			continue;
		}

	}

}
		
void name_broadcast(char * name, struct sockaddr_in * addr)
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

char * remove_newline(char * str)
{
    char * cut_nl = strchr(str, '\n');
    if (cut_nl) *cut_nl= '\0';
    return str;
}

void refresh_list(WINDOW * list_win, net_users_t * root, int force_refresh)
{
    net_users_t * cursor = root;
    static time_t before = 0;
    time_t difference = 0;
    if (!before || force_refresh)
    {
        before = time(NULL);
        wclear(list_win);
        getyx(list_win, cur_posY, cur_posX);
        //print list here
        do
        {
            wprintw(list_win,"%s", cursor->name);
            wmove(list_win,	++cur_posY, cur_posX);
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
            wmove(list_win,	++cur_posY, cur_posX);
        }
        while (cursor !=NULL);
        wrefresh(list_win);
        return;
    }
    return;
}

