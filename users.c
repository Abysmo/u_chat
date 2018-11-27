#include "users.h"


const struct net_user_list * list_init(const char * name, const char * ip)
{
    struct net_user_list * x = (struct net_user_list*)malloc(sizeof(struct net_user_list));
    strncpy(x->ip, ip, IP_LEN);
    strncpy(x->name, name, NAME_LEN);
    x->refresh_time = time(NULL);
    x->next = NULL;
    return x;
}


struct net_user_list * add_user (const struct net_user_list * root, char * name, char * ip)
{
    struct net_user_list * cursor = root;
    struct net_user_list * x = (struct net_user_list*)malloc(sizeof(struct net_user_list));
    strncpy(x->ip, ip, IP_LEN);
    strncpy(x->name, name, NAME_LEN);
    x->refresh_time = time(NULL);
    x->next = NULL;

    while (cursor->next != NULL)
    {
        if (strstr(cursor->ip,ip)) {cursor->refresh_time = time(NULL); free(x); return NULL;} //if user already exist in list, refresh timer.
        //if (strstr(root->name,name)) {free(x); return NULL;}
        cursor = cursor->next;
    }

    cursor->next = x;

    return x;
}


void delete_timeout_users(const struct net_user_list * root)
{
    struct net_user_list * cursor = root; //get start pos
    cursor = root->next; //go to next user
    time_t before, difference;

    while (cursor->next != NULL)
    {
        before = cursor->refresh_time;
        difference = time(NULL) - before;
        if (difference >= USER_TIMEOUT_S)
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




