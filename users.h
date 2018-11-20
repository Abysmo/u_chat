#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define IP_LEN 16
#define NAME_LEN 16


struct net_user
{
    char ip[IP_LEN];
    char name[NAME_LEN];
    clock_t refresh_time;
};
