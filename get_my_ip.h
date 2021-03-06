#ifndef _GETMYIP_H 
#define _GETMYIP_H 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#define IPV4_ADDR_LEN 16 //XXX.XXX.XXX.XXX + \0
#define ROUTE_PATH "/proc/net/route"

/*retflag ---> if (retflag == true) - getmyip() return actual ip, else return broadcast ip (xxx.xxx.xxx.255)
infoflag ---> if (infoflag == true) - getmyip() print info about local ip, broadcast ip and default interface else no info is printed*/
const char * getmyip(int retflag, int infoflag);

#endif
