#include "users.h"


struct net_user network[254];

struct net_user create_user ( char * name, char * ip)
{
    struct net_user x;
    strncpy(x.ip, ip, IP_LEN);
    strncpy(x.name, name, NAME_LEN);
    return x;
}
