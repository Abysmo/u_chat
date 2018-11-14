#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h> //exit
#include <stdio.h> //printf
#include <unistd.h> //close
#include <string.h>
#include <errno.h>
#include "get_my_ip.h"
//#include "terminal_ctl.h"
#include "terminal_ui.h"

//#define ADDR "192.168.0.255" // for inet_addr()
//#define ADDR(A,B,C,D) ((A<<24) | (B << 16) | (C << 8) | (D)) //for htonl()

#define PORT 5050
#define MSG_MAXLEN 1024
#define STD_OUT_INVITE ">> "
#define STD_IN_INVITE "<< "
#define IP_LEN 16
#define NAME_LEN 12


struct net_user 
{
	char ip[IP_LEN];
	char name[NAME_LEN];
};




int main()
{

	unsigned int income_addr_len;
	struct sockaddr_in addr, income_addr;
	char buf[MSG_MAXLEN];
	char msg[MSG_MAXLEN]={0};	
	int bytes_read;	
	
	//ncurses
	ncurses_setup();
	WINDOW * win1;	
	WINDOW * win2;
	win1 = create_msgbox_win();
	win2 = create_msgsend_win();


	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP
    if(sock < 0)
    {
        fprintf(stderr,"Socket error : %s \n", strerror(errno));
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
	
	const char * bc_ip = getmyip(0,0);
	if (bc_ip == NULL) {fprintf(stderr,"Failed to set IP ! \n"); exit(1);}
	addr.sin_addr.s_addr = inet_addr(bc_ip); 

	//bradcast premission
	unsigned int broadcastPermission = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
    fprintf(stderr,"Set Socket option error : %s \n", strerror(errno));

	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
	    fprintf(stderr,"Binding Socket error : %s \n", strerror(errno));
	    exit(1);
	}


	while (1)
	{	

		send_msg_handler(win2);


		if (fgets(msg,MSG_MAXLEN,stdin)!=NULL)
			{
	   			sendto(sock, msg, MSG_MAXLEN, 0,(struct sockaddr *)&addr, sizeof(addr));
				memset(msg, '\0', MSG_MAXLEN);
			}



    	bytes_read = recvfrom(sock, buf, MSG_MAXLEN, MSG_DONTWAIT, (struct sockaddr *)&income_addr, &income_addr_len);		
		if(bytes_read == 0 || bytes_read == EAGAIN) {continue;}
		else 
		{
			//if (strstr(inet_ntoa(income_addr.sin_addr), my_ip) != NULL){continue;} //if its my msg -> skip
			
			wprintw(win1,"%s %s",inet_ntoa(income_addr.sin_addr),buf);
			wrefresh(win1);		
			continue;
		/*
		fputs(inet_ntoa(income_addr.sin_addr), stdout); //received ip
		fputs(STD_OUT_INVITE, stdout);
		fputs(buf, stdout);
		memset(buf, '\0', strlen(buf));
		*/
		}

	}

}
		

		


