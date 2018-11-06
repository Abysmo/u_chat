#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> /* for strncpy */
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#define IPV4_ADDR_LEN 16 //XXX.XXX.XXX.XXX + \0

int main()
{

	/*GET DEFAULT interface*/

	FILE *f;
	char line[100] , *p , *c;

	f = fopen("/proc/net/route" , "r");
	if (f == NULL)
	{
		printf("fail to open '/proc/net/route' : %s \n", strerror(errno));
		printf("enter your IP in format XXX.XXX.XXX.XXX \n");
		char stdin_ip[IPV4_ADDR_LEN]={0};
		scanf("%s", stdin_ip);
		char alt_local_ip[IPV4_ADDR_LEN + 2] = {0};	 //+2 because  + ""
		sprintf(alt_local_ip,"\"%s\"",stdin_ip);
		printf("%s\n", alt_local_ip);

		return 0;
	}


	while(fgets(line , 100 , f))
	{
		p = strtok(line , " \t");
		c = strtok(NULL , " \t");
	
		if(p!=NULL && c!=NULL)
		{
			if(strcmp(c , "00000000") == 0)
			{
				printf("Default interface is : %s \n" , p);
				break;
			}
		}
	}

	/*get local IP*/

	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
    {
        printf("Creating socket error : %s \n", strerror(errno));
        exit(1);
    }
	/*get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* IP address attached to default interface */
	strncpy(ifr.ifr_name, p, IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	/* making broadcast ip */

	const char * local_ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

	char broadcast_ip[IPV4_ADDR_LEN] = {0};
	strncpy (broadcast_ip, local_ip, (sizeof(broadcast_ip)*(sizeof(char))));		

 	char * lastByte = strrchr(broadcast_ip, '.');
	memcpy(++lastByte, "255", (3*sizeof(char)));
	
	printf("local IP : %s\n", local_ip);
	printf("broadcast IP : %s\n", broadcast_ip);

	return 0;
}
