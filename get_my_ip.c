#include "get_my_ip.h"

char alt_local_ip[IPV4_ADDR_LEN + 2] = {0};	 //+2 because  + ""
char broadcast_ip[IPV4_ADDR_LEN] = {0};
char local_ip[IPV4_ADDR_LEN] = {0};

/*retflag ---> if (retflag == true) - getmyip() return actual ip, else return broadcast ip (xxx.xxx.xxx.255)
infoflag ---> if (infoflag == true) - getmyip() print info about local ip, broadcast ip and default interface else no info is printed*/
const char * getmyip(int retflag, int infoflag)
{

	/*GET DEFAULT interface*/

	FILE *f;
	char line[100] , *p , *c;

    f = fopen(ROUTE_PATH , "r");
	if (f == NULL)
	{
		printf(ROUTE_PATH);
		fprintf(stderr," - Fail to open : %s \n", strerror(errno));
		printf("Enter your IP in format XXX.XXX.XXX.XXX \n");
		char stdin_ip[IPV4_ADDR_LEN]={0};
		scanf("%s", stdin_ip);
		
        sprintf(alt_local_ip,"\"%s\"",stdin_ip); //concat. with quotation marks
		
		
		int ip_sep = 4;
		char ip_chk[IPV4_ADDR_LEN + 2];
		memcpy (ip_chk, alt_local_ip, strlen(alt_local_ip));
		if ((strtok(ip_chk, ".")) == NULL)
		{
			fprintf(stderr,"IP format is wrong ! \n");
			return NULL;
		}
		else
		{
			ip_sep--;
			while ((strtok(NULL,".")) != NULL)
			ip_sep--;
			if (ip_sep !=0)
			{
				fprintf(stderr,"IP format is wrong !\n");
				return NULL;
			}
		}
		
		printf("%s\n", alt_local_ip);
		printf("OK! \n");
		return alt_local_ip; 
	}

    //parsing ROUTE_PATH("/proc/net/route") for default interface
	while(fgets(line , 100 , f))
	{
		p = strtok(line , " \t");
		c = strtok(NULL , " \t");
	
		if(p!=NULL && c!=NULL)
		{
			if(strcmp(c , "00000000") == 0)
			{
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
        fprintf(stderr,"Creating temp socket error : %s \n", strerror(errno));
        return NULL;
    }
	/*get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* IP address attached to default interface */
	strncpy(ifr.ifr_name, p, IFNAMSIZ-1);

/*
        SIOCGIFADDR
        Get  or set the address of the device using ifr_addr.  Setting
        the interface address is a privileged operation.  For compati‐
        bility, only AF_INET addresses are accepted or returned.
 */
	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	/* making broadcast ip */

    strcpy (local_ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	strncpy (broadcast_ip, local_ip, (sizeof(broadcast_ip)*(sizeof(char))));		

 	char * lastByte = strrchr(broadcast_ip, '.');
	memcpy(++lastByte, "255", (3*sizeof(char)));
	
	if(infoflag)
	{
		printf("Default interface is : %s \n" , p);
		printf("local IP : %s\n", local_ip);
		printf("broadcast IP : %s\n", broadcast_ip);
	}
	
	if (retflag) return local_ip;
	else return broadcast_ip;
}

