#include "get_my_ip.h"

#define IPV4_ADDR_LEN 16 //XXX.XXX.XXX.XXX + \0

char alt_local_ip[IPV4_ADDR_LEN + 2] = {0};	 //+2 because  + ""
char broadcast_ip[IPV4_ADDR_LEN] = {0};

const char * getmyip()
{

	/*GET DEFAULT interface*/

	FILE *f;
	char line[100] , *p , *c;

	f = fopen("/proc/net/route1111" , "r"); // TEST
	if (f == NULL)
	{
		printf("Fail to open '/proc/net/route' : %s \n", strerror(errno));
		printf("Enter your IP in format XXX.XXX.XXX.XXX \n");
		char stdin_ip[IPV4_ADDR_LEN]={0};
		scanf("%s", stdin_ip);
		
		sprintf(alt_local_ip,"\"%s\"",stdin_ip);
		
		
		int ip_sep = 4;
		char ip_chk[IPV4_ADDR_LEN + 2];
		memcpy (ip_chk, alt_local_ip, strlen(alt_local_ip));
		if ((strtok(ip_chk, ".")) == NULL)
		{
			printf("IP format is wrong ! \n");
			return NULL;
		}
		else
		{
			ip_sep--;
			while ((strtok(NULL,".")) != NULL)
			ip_sep--;
			if (ip_sep !=0)
			{
				printf("IP format is wrong !\n");
				return NULL;
			}
		}
		
		printf("%s\n", alt_local_ip);
		printf("OK! \n");
		return alt_local_ip; 
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
        return NULL;
    }
	/*get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* IP address attached to default interface */
	strncpy(ifr.ifr_name, p, IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	/* making broadcast ip */

	const char * local_ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

	
	strncpy (broadcast_ip, local_ip, (sizeof(broadcast_ip)*(sizeof(char))));		

 	char * lastByte = strrchr(broadcast_ip, '.');
	memcpy(++lastByte, "255", (3*sizeof(char)));
	
	printf("local IP : %s\n", local_ip);
	printf("broadcast IP : %s\n", broadcast_ip);

	return broadcast_ip;
}