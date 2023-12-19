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

        // Encrypt data
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

int main(int argc, const char *argv[])
{
    signal(SIGINT, handle_exit);

    if (argc != 3)
        return -1;

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

    char buffer[65535] = {0};
    while (1)
    {
        // Receive data from server
        int received = ireceive(sock, buffer);
        if (received < 0)
        {
            printf("\nINFO: server has been closed.\n");
            handle_exit();
        }

#ifdef ENABLE_CRYPTO

        // Try to decrypt data, close connection if failed
        char decrypted[65535] = {0};
        int length = decrypt_aes128((const unsigned char *)buffer, received, ENC_AES_KEY, (unsigned char *)decrypted);
        if (length < 0)
        {
            handle_exit();
        }
        printf("%s", decrypted);

#else

        printf("%s", buffer);

#endif

        // Clear buffer
        memset(buffer, 0, received);
    }

    return 0;
}
