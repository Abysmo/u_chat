#include "users.h"


net_users_t * list_init(const char * name, const char * ip)
{
    net_users_t * x = (net_users_t*)malloc(sizeof(net_users_t));
    strncpy(x->ip, ip, IP_LEN);
    strncpy(x->name, name, NAME_LEN);
    x->refresh_time = time(NULL);
    x->next = NULL;
    return x;
}


net_users_t * add_user (net_users_t * root, char * name, char * ip)
{
    net_users_t * cursor = root;
    net_users_t * x = (net_users_t*)malloc(sizeof(net_users_t));
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



void delete_timeout_users(net_users_t * root)
{
    net_users_t * cursor = root; //get start pos
    cursor = root->next; //go to next user
    time_t before, difference;

    while (cursor->next != NULL)
    {
        before = cursor->refresh_time;
        difference = time(NULL) - before;
        if (difference >= USER_TIMEOUT_S)
        {
            net_users_t * tmp = root; //second cursor for changing previous poiner
            while (tmp->next != cursor) //locate cursor pos
                tmp = tmp->next; //go to next struct
            tmp->next = cursor->next; // change pointers
            free(cursor); //free memory
        }
        cursor = cursor->next;
    }
}




