#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/*
The data packet structure will be:
| length (2 bytes) | encrypted data (0-65535 bytes) |

The first byte indicates length of packet, remote host will respond until received enough data.
Maxsize of packet will be 2 * 2 ** 8 = 65536 bytes.

Once client closed socket, any operation on socket will return 0, then the socket will be closed.
*/
typedef int (*SocketOperator)(int, const struct sockaddr *, socklen_t);
int iconnect(const char *, in_port_t, SocketOperator);
int ireceive(int, void *);
int isend(int, void *, unsigned short);
