#include "users.h"


struct net_user_list * list_root;


struct net_user_list * list_init(char * name, char * ip)
{
    struct net_user_list * x = (struct net_user_list*)malloc(sizeof(struct net_user_list));
    strncpy(x->ip, ip, IP_LEN);
    strncpy(x->name, name, NAME_LEN);
    x->refresh_time = clock();
    x->next = NULL;
    return x;
}


struct net_user_list * add_user (struct net_user_list * root, char * name, char * ip)
{
    struct net_user_list * x = (struct net_user_list*)malloc(sizeof(struct net_user_list));
    strncpy(x->ip, ip, IP_LEN);
    strncpy(x->name, name, NAME_LEN);
    x->refresh_time = clock();
    x->next = NULL;

    while (root->next != NULL)
        root = root->next;
    root->next = x;

    return x;
}


void delete_timeout_users(struct net_user_list * root)
{
    struct net_user_list * cursor = root; //get start pos
    cursor = root->next; //go to next user
    clock_t before, difference;
    unsigned int refresh_sec;

    while (cursor->next != NULL)
    {
        before = cursor->refresh_time;
        difference = clock() - before;
        refresh_sec = difference / CLOCKS_PER_SEC;
        if (refresh_sec >= USER_TIMEOUT_S)
        {
            struct net_user_list * tmp = root; //second cursor for changing previous poiner
            while (tmp->next != cursor) //locate cursor pos
                tmp = tmp->next; //go to next struct
            tmp->next = cursor->next; // change pointers
            free(cursor); //free memory
        }
        cursor = cursor->next;
    }
}
