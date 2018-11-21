#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define IP_LEN 16
#define NAME_LEN 16
#define MAX_USERS 255


struct net_user_list
{
    struct  net_user_list * next;
    char ip[IP_LEN];
    char name[NAME_LEN];
    clock_t refresh_time;
};



