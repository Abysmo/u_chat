#include <sys/types.h>
#include <signal.h> //kill();
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

int split = 0; //fork(); result for kill();

struct net_user 
{
	char ip[IP_LEN];
	char name[NAME_LEN];
};

void sender()
{

    int sock1;
	char msg[MSG_MAXLEN]={0};
    struct sockaddr_in addr1;

	//ncurses
	win1_outcome_msg_init();

	sock1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP
    if(sock1 < 0)
    {
        printf("Socket error : %s \n", strerror(errno));
        exit(1);
    }

    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(PORT);
	
	const char * bc_ip = getmyip(0,0);
	if (bc_ip == NULL) {printf("Failed to set IP ! \n"); kill(split, 2); exit(1);}
	addr1.sin_addr.s_addr = inet_addr(bc_ip); 

	//bradcast premission
	unsigned int broadcastPermission = 1;
	if (setsockopt(sock1, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
    printf("Set Socket option error : %s \n", strerror(errno));

	while (1)
	{	

		wprintw(win,"TEST_WIN");
		wrefresh(win);		
	
		fputs(STD_IN_INVITE, stdout); //invitation string
		if (fgets(msg,MSG_MAXLEN,stdin)!=NULL)
			{
	   			sendto(sock1, msg, MSG_MAXLEN, 0,(struct sockaddr *)&addr1, sizeof(addr1));
				memset(msg, '\0', MSG_MAXLEN);
			}
    }

    close(sock1);
}


int main()
{

	split = fork();

	if (split == -1) // if fork fail
	    {
		    //print err
		    printf("fork() error - %s\n", strerror(errno));
		   	exit(1);
	    }

	//child - listen server
	else if (split == 0)
	{
		unsigned int sock; 
		unsigned int income_addr_len;
		struct sockaddr_in addr, income_addr;
		char buf[MSG_MAXLEN];
		int bytes_read;	

		//ncurses 
		win3_income_msg_init();

		const char * tmp_my_ip = getmyip(1,0);
		char my_ip[IP_LEN]={0};
		strncpy(my_ip,tmp_my_ip, strlen(tmp_my_ip));

		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(sock < 0)
		{
		    printf("Socket error : %s \n", strerror(errno));
		    exit(1);
		}
		
		addr.sin_family = AF_INET;
		addr.sin_port = htons(PORT);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
		    printf("Binding Socket error : %s \n", strerror(errno));
		    exit(1);
		}

		while(1)
		{
		    bytes_read = recvfrom(sock, buf, MSG_MAXLEN, 0, (struct sockaddr *)&income_addr, &income_addr_len);
		    if(bytes_read == 0) {continue;}
			else if (bytes_read == -1) 
			{
				printf("Socket failure : %s \n", strerror(errno));
				exit(1);
			}
			else 
			{
				if (strstr(inet_ntoa(income_addr.sin_addr), my_ip) != NULL){continue;}
				

				wprintw(win,"%s %s",inet_ntoa(income_addr.sin_addr),buf);
				wrefresh(win);		
				/*
				fputs(inet_ntoa(income_addr.sin_addr), stdout); //received ip
				fputs(STD_OUT_INVITE, stdout);
				fputs(buf, stdout);
				memset(buf, '\0', strlen(buf));
				*/

			}
		}
		
		return 0;
		close(sock);
		
	}
	
	//parent - sender
	else 
	{
		sender();
		return 0;
	}

}
