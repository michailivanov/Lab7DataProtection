#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>

#include "isock.h"
#include "icrypto.h"

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

void serve_server(int sock)
{
    char buffer[65535] = {0};
    while (1)
    {
        // Receive data from server
        int received = ireceive(sock, buffer);
        if (received < 0)
        {
            printf("\nINFO: remote host has been closed.\n");
            return;
        }

#ifdef ENABLE_CRYPTO

        // Try to decrypt data, close connection if failed
        char decrypted[65535] = {0};
        int length = decrypt_aes128((const unsigned char *)buffer, received, ENC_AES_KEY, (unsigned char *)decrypted);
        if (length < 0)
        {
            return;
        }
        printf("%s", decrypted);

#else

        printf("%s", buffer);

#endif

        // Clear buffer
        memset(buffer, 0, received);
    }
}

int main(int argc, const char *argv[])
{

    signal(SIGINT, handle_exit);
    signal(SIGCHLD, handle_exit);
    int sock = iconnect(argv[1], atoi(argv[2]), connect);
    if (sock < 0)
    {
        close(sock);
        return -1;
    }

    SOCK = sock;

    // Fork a new process for receiving data
    int pid = fork();
    if (pid < 0)
    {
        close(sock);
        perror("ERROR: fork receving process failed\n");
        return -2;
    }
    if (pid == 0)
    {
        serve_server(sock);
        exit(0);
    }

    // Continously reading data from console and send
    char buffer[65535] = {0};
    while (1)
    {
        printf(">> ");
        fgets(buffer, 1024, stdin);
        unsigned short size = strlen(buffer);

#ifdef ENABLE_CRYPTO

        // Encrypt data
        unsigned char encrypted[65535] = {0};
        int length = encrypt_aes128((const unsigned char *)buffer, size, ENC_AES_KEY, encrypted);
        isend(SOCK, encrypted, length);

#else

        isend(SOCK, buffer, size);

#endif

        // Sleep a second for reprint >> mark
        memset(buffer, 0, size);
        sleep(1);
    }

    return 0;
}
