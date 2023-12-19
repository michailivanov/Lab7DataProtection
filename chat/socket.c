#include "socket.h"

int iconnect(const char *address, in_port_t port, SocketOperator operate)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("ERROR: create socket failed\n");
        return -1;
    }

    struct sockaddr_in sock_address;
    memset(&sock_address, 0, sizeof(sock_address));
    sock_address.sin_port = htons(port);
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = inet_addr(address);

    socklen_t address_len = sizeof(sock_address);
    if (operate(sock, (struct sockaddr *)&sock_address, address_len) < 0)
    {
        fprintf(stderr, "ERROR: connect to address %s:%i failed\n", address, port);
        return -2;
    }

    return sock;
}

int ireceive(int target, void *buffer, size_t length)
{
    int received = 0;
    while (received < length)
    {
        int ur = recv(target, (char *)buffer + received, length - received, 0);
        if (ur <= 0)
        {
            close(target);
            return -1;
        }
        received += ur;
    }
    return received;
}

int isend(int target, void *buffer, size_t length)
{
    int sent = 0;
    while (sent < length)
    {
        int us = send(target, (char *)buffer + sent, length - sent, 0);
        if (us <= 0)
        {
            close(target);
            return -1;
        }
        sent += us;
    }
    return sent;
}
