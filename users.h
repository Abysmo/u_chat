#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define IP_LEN 16
#define NAME_LEN 16
#define MAX_USERS 255
#define USER_TIMEOUT_S 10 //user timeout in seconds

struct net_user_list
{
    struct  net_user_list * next;
    char ip[IP_LEN];
    char name[NAME_LEN];
    clock_t refresh_time;
};

struct net_user_list * list_init(const char * name, const char * ip);
struct net_user_list * add_user (struct net_user_list * root, char * name, char * ip);
void delete_timeout_users(struct net_user_list * root);
