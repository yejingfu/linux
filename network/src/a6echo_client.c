#include "unp.h"

/**
 * Use select() to implement the I/O duplex...
 * The I/O includes stdin and socket_fd
 */

void str_cli(FILE *fp, int sockfd)
{
    int  filefd = fileno(fp);
    int  maxfd;
    int  stdineof;
    char buff[MAXLINE];

    fd_set rset;                // the set of fd for reading
    int    n;

    FD_ZERO(&rset);
    stdineof = 0;               // 0 means the app is ready for reading from stdin. 1 means the stdin is closed.

    for (; ;) {
        if (stdineof == 0)
            FD_SET(filefd, &rset);
        FD_SET(sockfd, &rset);
        maxfd = max(filefd, sockfd) + 1;

        // Hang here until any fd in the rset is waken up.
        // The rset is in-out parameter
        Select(maxfd, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            // The socket is ready for read
            if ((n = Read(sockfd, buff, MAXLINE)) == 0) {
                // no data is read
                if (stdineof == 1) {
                    // The stdin has already exited
                    return;
                } else {
                    err_quit("str_cli: Server terminated prematurely.");
                }
            }
            // output to stdout
            Write(fileno(stdout), buff, n);
        }

        if (FD_ISSET(filefd, &rset)) {
            // Ready to read from stdin
            if ((n = Read(filefd, buff, MAXLINE)) == 0) {
                // Ctrl-D to stop input from stdin
                
                stdineof = 1;
                // half-close the socket, now it cannot write but can read.
                // It also send FIN to server.
                Shutdown(sockfd, SHUT_WR);

                FD_CLR(filefd, &rset);
                continue;
            }

            Write(sockfd, buff, n); // write to socket
        }
    }
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        err_quit("Usage: a6echo_client.out <IP>");
    }

    int                sockfd;
    struct sockaddr_in addr;

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &addr.sin_addr);

    Connect(sockfd, (SA*)&addr, sizeof(addr));
    str_cli(stdin, sockfd);
    
    exit(0);
}

