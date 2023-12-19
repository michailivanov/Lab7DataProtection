#include "socket.h"
#define MAX_MESSAGE_LENGTH 80

/* Create a socket and try to bind it to given address.

Returns:
    socket_file_handler if created and bound successfully
    -1                  if create socket failed
    -2                  if bind socket to address failed
*/
int iconnect(const char *address, in_port_t port, SocketOperator operate)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0); // Create socket with IPv4 and TCP option
    if (sock < 0)
    {
        perror("ERROR: create socket failed\n");
        return -1;
    }

    // Make socket address structs
    struct sockaddr_in sock_address;
    memset(&sock_address, 0, sizeof(sock_address));
    sock_address.sin_port = htons(port);
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = inet_addr(address);

    // Try to bind socket to given address
    socklen_t address_len = sizeof(sock_address);
    if (operate(sock, (struct sockaddr *)&sock_address, address_len) < 0)
    {
        fprintf(stderr, "ERROR: connect to address %s:%i failed\n", address, port);
        return -2;
    }

    return sock;
}

int ireceive(int target, void *buffer)
{
    int received = 0;
    while (received < MESSAGE_SIZE)
    {
        int ur = recv(target, (char *)buffer + received, MESSAGE_SIZE - received, 0);
        if (ur <= 0)
        {
            close(target);
            return -1;
        }
        received += ur;
    }
    return received;
}

int isend(int target, void *buffer, unsigned short length)
{
    // Дополнение сообщения до 80 символов
    char paddedMessage[MESSAGE_SIZE];
    memset(paddedMessage, 0, MESSAGE_SIZE);
    memcpy(paddedMessage, buffer, length > MESSAGE_SIZE ? MESSAGE_SIZE : length);

    int sent = 0;
    while (sent < MESSAGE_SIZE)
    {
        int us = send(target, paddedMessage + sent, MESSAGE_SIZE - sent, 0);
        if (us <= 0)
        {
            close(target);
            return -1;
        }
        sent += us;
    }
    return sent;
}
