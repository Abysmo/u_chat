#include "users.h"


struct net_user network[MAX_USERS]={0};
unsigned int network_pos = 0;

int check_user (char * service_msg)
{
    char * p_result = NULL;
    char * name = (service_msg + 1);

    for (int i = 0; i < MAX_USERS; i++)
    {
        p_result = strstr(network[i].name,name);
        if (*(&network[i].name) == '\0') return 0;
        else if (p_result != NULL) return 0;
    }
    return 1;
}

struct net_user create_user (char * name, char * ip)
{
    struct net_user x;
    strncpy(x.ip, ip, IP_LEN);
    strncpy(x.name, name, NAME_LEN);
    x.refresh_time = clock();
    return x;
}

void sort_users()
{
    struct net_user tmp;

    for (int i = 0; i < MAX_USERS; i++)
    {
        for (;(*(&network[i].name) == '\0') && (*(&network[i+1].name) != '\0');i--)
        {
            tmp = network[i];
            network[i] = network[i+1];
            network[i+1] = tmp;
        }

    }

}
