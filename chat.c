#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h> //exit
#include <stdio.h> //printf
#include <unistd.h> //close
#include <string.h>
#include <errno.h>


#define ADDR "192.168.0.255" // for inet_addr()
//#define ADDR(A,B,C,D) ((A<<24) | (B << 16) | (C << 8) | (D)) //for htonl()
#define ADDR2 4294967295 //192.168.0.255(3232235775) | INADDR_LOOPBACK | 4294967295(255.255.255.255)
#define PORT 5050

char msg[1024]={0};

void sender()
{
    int sock1;
    struct sockaddr_in addr1;

    //sock1 = socket(AF_INET, SOCK_STREAM, 0); //TCP
	sock1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP
    if(sock1 < 0)
    {
        printf("Socket error : %s \n", strerror(errno));
        exit(1);
    }

    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(PORT); // или любой другой порт...
    addr1.sin_addr.s_addr = inet_addr(ADDR);

	//bradcast premission
	unsigned int broadcastPermission = 1;
	if (setsockopt(sock1, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
    printf("setsockopt() failed");


/*	
	if(bind(sock1, (struct sockaddr *)&addr1, sizeof(addr1)) < 0)
	{
	    printf("Bind address error : %s \n", strerror(errno));
	    exit(2);
	}

    if(connect(sock1, (struct sockaddr *)&addr1, sizeof(addr1)) < 0)
    {
        printf("Connect error : %s \n", strerror(errno));
        exit(2);
    }
*/	
	while (1)
	{
		//printf("\nEnter msg: ");
		scanf("%s", msg);
	   	//send(sock1, msg, sizeof(msg), 0); //TCP
		sendto(sock1, msg, sizeof(msg), 0,(struct sockaddr *)&addr1, sizeof(addr1));//UDP
		memset(msg, '\0', sizeof(msg));
    }

    close(sock1);
}


int main()
{
	int split = fork();

	if (split == -1) // if fork fail
	    {
		    //print err
		    printf("fork() error - %s\n", strerror(errno));
		   	exit(1);
	    }

	//child - sender
	else if (split == 0)
	{
		//return 0; //test

		sender();
	}
	//parent - listen server
	else 
	{

		int sock;
		struct sockaddr_in addr;
		char buf[1024];
		int bytes_read;
/*//TCP
		int listener = socket(AF_INET, SOCK_STREAM, 0);
		if(listener < 0)
		{
		    printf("Socket error : %s \n", strerror(errno));
		    exit(1);
		}
		
		addr.sin_family = AF_INET;
		addr.sin_port = htons(50505);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		

		if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
		    printf("Bind address error : %s \n", strerror(errno));
		    exit(2);
		}

		listen(listener, 1);
		
	

		while(1)
		{
		    sock = accept(listener, NULL, NULL);
		    if(sock < 0)
		    {
		        printf("Accept connection error : %s \n", strerror(errno));
		        exit(3);
		    }

		    while(1)
		    {
		        bytes_read = recv(sock, buf, 1024, 0);
		        if(bytes_read == 0) {continue;}
				else if (bytes_read == -1) 
					{
						printf("Cannot read data from sock : %s \n", strerror(errno));
						exit(4);
					}
				else printf("%s\n",buf);
		    }
		    
		}
*/

//UDP
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
		    bytes_read = recvfrom(sock, buf, 1024, 0, NULL, NULL);
		    if(bytes_read == 0) {continue;}
			else if (bytes_read == -1) 
				{
					printf("Cannot read data from sock : %s \n", strerror(errno));
					exit(4);
				}
			else printf("%s\n",buf);
		}
		
		return 0;
		close(sock);
		
	}

}
