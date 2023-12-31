#include "socket.h"

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

/* Try to receive assigned bytes from remote host.

This function will block process when it got insuffient data from client.
Try to read 2 bytes as indicator of message length form remote host then message body.

Returns:
    message_size if received successfully.
    -1           if client already closed socket.
*/
int ireceive(int target, void *buffer)
{
    int received = 0;
    unsigned short expected;
    while (received < 2)
    {
        int ur = 0;
        if ((ur = recv(target, (&expected) + received, 2 - received, 0)) == 0)
        {
            close(target);
            return -1;
        }
        received += ur;
    }
    received = 0;
    while (received < expected)
    {
        int ur = 0;
        if ((ur = recv(target, buffer, expected - received, 0)) == 0)
        {
            memset(buffer, 0, received);
            close(target);
            return -1;
        }
        received += ur;
    }
    return received;
}

/* Try to send message to remote host.

This function will send 2 bytes head as indicator of message length then message body.

See `ireceive` function.

Returns:
    message_size if send successfully.
    -1           if client already closed socket.
*/
int isend(int target, void *buffer, unsigned short length)
{
    int sent = 0;
    while (sent < 2)
    {
        int us = 0;
        if ((us = send(target, (&length) + sent, 2 - sent, 0)) == 0)
        {
            close(target);
            return -1;
        }
        sent += us;
    }
    sent = 0;
    while (sent < length)
    {
        int us = 0;
        if ((us = send(target, buffer, length - sent, 0)) == 0)
        {
            close(target);
            return -1;
        }
        sent += us;
    }
    return sent;
}
