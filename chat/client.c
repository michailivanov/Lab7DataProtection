#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "socket.h"
#include "encryption.h"

#ifdef ENABLE_CRYPTO
static unsigned char ENC_AES_KEY[] = {0, 10, 25, 43, 4, 135, 246, 78, 118, 29, 101, 156, 126, 16, 15, 1};
#endif

static int SOCK = -1;

void handle_exit()
{
    close(SOCK);
    shutdown(SOCK, SHUT_RDWR);
    exit(0);
}

void send_to_server(int sock)
{
    char buffer[65535] = {0};
    while (1)
    {
        printf(">> ");
        fgets(buffer, sizeof(buffer), stdin);
        unsigned short size = strlen(buffer);

#ifdef ENABLE_CRYPTO

        unsigned char encrypted[65535] = {0};
        int length = encrypt_aes128((const unsigned char *)buffer, size, ENC_AES_KEY, encrypted);
        isend(sock, encrypted, length);

#else

        isend(sock, buffer, size);

#endif

        // Clear buffer
        memset(buffer, 0, size);
    }
}

void receive_from_server(int sock)
{
    char buffer[65535] = {0};
    while (1)
    {
        int received = ireceive(sock, buffer);
        if (received < 0)
        {
            printf("\nINFO: server has been closed.\n");
            handle_exit();
        }

#ifdef ENABLE_CRYPTO

        char decrypted[65535];
        int length = decrypt_aes128((const unsigned char *)buffer, received, ENC_AES_KEY, (unsigned char *)decrypted);
        if (length < 0)
        {
            handle_exit();
        }

        // Process the received message (in decrypted form) if needed

#else

        // Process the received message if needed

#endif

        // Example: Print the received message
        printf("Received message from server: %s\n", decrypted);

        // Clear buffer
        memset(buffer, 0, received);
    }
}

int main(int argc, const char *argv[])
{
    signal(SIGINT, handle_exit);
    int sock = iconnect(argv[1], atoi(argv[2]), connect);
    if (sock < 0)
    {
        close(sock);
        return -1;
    }

    SOCK = sock;

    // Fork a new process for sending data to the server
    int sender_pid = fork();
    if (sender_pid < 0)
    {
        close(sock);
        perror("ERROR: fork sender process failed\n");
        return -2;
    }
    if (sender_pid == 0)
    {
        send_to_server(sock);
        exit(0);
    }

    // Receive data from the server
    receive_from_server(sock);

    return 0;
}
