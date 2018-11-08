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

//#define ADDR "192.168.0.255" // for inet_addr()
//#define ADDR(A,B,C,D) ((A<<24) | (B << 16) | (C << 8) | (D)) //for htonl()

#define PORT 5050
#define MSG_MAXLEN 1024

//char msg[MSG_MAXLEN]={0};
int split = 0; //fork(); result for kill();

struct net_user 
{
char ip[12];
char name[12];
};

void sender()
{
    int sock1;
	char msg[MSG_MAXLEN]={0};
    struct sockaddr_in addr1;

	sock1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP
    if(sock1 < 0)
    {
        printf("Socket error : %s \n", strerror(errno));
        exit(1);
    }

    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(PORT);
	
	const char * badr = getmyip();
	if (badr == NULL) {printf("Failed to set IP ! \n"); kill(split, 2); exit(1);}
	addr1.sin_addr.s_addr = inet_addr(badr); 

	//bradcast premission
	unsigned int broadcastPermission = 1;
	if (setsockopt(sock1, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
    printf("Set Socket option error : %s \n", strerror(errno));

	while (1)
	{
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
		int sock;
		struct sockaddr_in addr;
		char buf[MSG_MAXLEN];
		int bytes_read;

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
		    exit(2);
		}

		while(1)
		{
		    bytes_read = recvfrom(sock, buf, MSG_MAXLEN, 0, NULL, NULL);
		    if(bytes_read == 0) {continue;}
			else if (bytes_read == -1) 
				{
					printf("Cannot read data from sock : %s \n", strerror(errno));
					exit(4);
				}
			else fputs(buf, stdout);
		}
		
		return 0;
		close(sock);
		
	}
	
	//parent - sender
	else 
	{
		sender();
	}

}
