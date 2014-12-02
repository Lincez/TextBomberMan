#include "../../include/LClasses/LClasses.h"


int winsockSend(SOCKET sock, const char* s, int i ,int j)
{
    return send(sock, s, i, j);
};
int winsockConnect(SOCKET sock, const struct sockaddr* addr, int i)
{
    return connect(sock, addr, i);
}
int winsockAccept(SOCKET sock, struct sockaddr* addr, int* i)
{
    return accept(sock, addr, i);
}
int winsockListen(SOCKET sock, int i)
{
    return listen(sock, i);
}
int winsockBind(SOCKET sock, const struct sockaddr* addr, int i)
{
    return bind(sock, addr, i);
}

