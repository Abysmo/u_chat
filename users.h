#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define IP_LEN 16
#define NAME_LEN 16
#define MAX_USERS 255
#define USER_TIMEOUT_S 10 //user timeout in seconds

typedef struct net_user_list
{
    struct net_user_list * next;
    char ip[IP_LEN];
    char name[NAME_LEN];
    time_t refresh_time;
}net_users_t;

net_users_t * list_init(const char * name, const char * ip);
net_users_t * add_user (net_users_t * root, char * name, char * ip);
void delete_timeout_users(net_users_t * root);
