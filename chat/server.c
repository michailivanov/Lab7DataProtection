#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>

#include "socket.h"
#include "encryption.h"

#define MAX_CHILD_PROCESS 64

#ifdef ENABLE_CRYPTO
static unsigned char ENC_AES_KEY[] = {0, 10, 25, 43, 4, 135, 246, 78, 118, 29, 101, 156, 126, 16, 15, 1};
#endif

static int CHILD_SOCK = -1;
static pid_t CHILDREN_PROCESS[MAX_CHILD_PROCESS] = {0};

int toggle_child_process_flag(pid_t from, pid_t to)
{
    for (int index = 0; index < MAX_CHILD_PROCESS; ++index)
    {
        if (CHILDREN_PROCESS[index] == from)
        {
            CHILDREN_PROCESS[index] = to;
            return index;
        }
    }
    return -1;
}

void handle_exit_in_parent()
{
    for (int index = 0; index < MAX_CHILD_PROCESS; ++index)
    {
        if (CHILDREN_PROCESS[index])
        {
            pid_t pid = CHILDREN_PROCESS[index];
            CHILDREN_PROCESS[index] = 0;
            kill(pid, SIGTERM);
            int killing_status;
            waitpid(pid, &killing_status, 0);
        }
    }
    exit(0);
}

void handle_defunct()
{
    pid_t pid;
    while ((pid = wait(NULL)) > 0)
    {
        toggle_child_process_flag(pid, 0);
    }
}

void handle_sigterm_in_child()
{
    close(CHILD_SOCK);
}

void serve_client(int client, struct sockaddr_in *addr)
{
    char data[65535];

    while (1)
    {
        int received = ireceive(client, data);
        if (received < 0)
            return;

#ifdef ENABLE_CRYPTO

        char decrypted[65535];
        int length = decrypt_aes128((const unsigned char *)data, received, ENC_AES_KEY, (unsigned char *)decrypted);
        if (length < 0)
            return;

        memset(data, 0, received);

        // Process the received message (in decrypted form) if needed

#else

        // Process the received message if needed

#endif

        // Example: Print the received message
        printf("Received message from client: %s\n", decrypted);

        // Respond to the client if needed

        // Clear the buffer
        memset(data, 0, received);
    }
}

int main(int argc, const char *argv[])
{
    signal(SIGINT, handle_exit_in_parent);
    signal(SIGCHLD, handle_defunct);

    if (argc != 3)
        return -1;

    int server = iconnect(argv[1], atoi(argv[2]), bind);
    if (server < 0 || listen(server, SOMAXCONN) < 0)
        return -2;

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        int client = accept(server, (struct sockaddr *)&client_addr, &client_addr_size);
        int pid = fork();

        if (pid < 0)
        {
            close(client);
            perror("ERROR: fork process failed\n");
            return -2;
        }

        if (pid == 0)
        {
            close(server);
            CHILD_SOCK = client;
            signal(SIGTERM, handle_sigterm_in_child);
            serve_client(client, &client_addr);
            exit(0);
        }

        if (pid > 0)
        {
            close(client);
            int collected = toggle_child_process_flag(0, pid);
            if (collected == -1)
            {
                kill(pid, SIGTERM);
                perror("ERROR: client request has been declined due to maximum registered clients\n");
            }
        }
    }

    return 0;
}
