#include "unp.h"
#include <limits.h> // the poll() uses OPEN_MAX

/**
 * use select() or poll() to implement the I/O duplex...
 */

void server_select()
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;

    int       nready;
    int       client[FD_SETSIZE];

    struct sockaddr_in cliaddr, srvaddr;
    socklen_t          clen;
    
    ssize_t n;
    fd_set  rset, allset;

    char buff[MAXLINE];
    char buff2[MAXLINE + 10];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&srvaddr, sizeof(srvaddr));
    srvaddr.sin_family      = AF_INET;
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvaddr.sin_port        = htons(SERV_PORT);

    Bind(listenfd, (SA*)&srvaddr, sizeof(srvaddr));

    Listen(listenfd, LISTENQ);  // set max of listen quueue

    maxfd = listenfd;
    maxi  = -1;
    
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for (; ;) {
        // keep allset from untouching, and suspend on the rset
        rset   = allset;
        nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

        printf("Selected: %d\n", nready);

        if (FD_ISSET(listenfd, &rset)) {
            // New client is comming, get its connection fd
            clen   = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA*)&cliaddr, &clen);

            // store the connfd into the client array
            for (i = 0; i< FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = connfd;
                    break;
                }
            }
            if (i == FD_SETSIZE) {
                // no more available fd in the client
                err_quit("too many client.");
            }

            // store the new connfd into the allset
            FD_SET(connfd, &allset);
            if (connfd > maxfd)
                maxfd = connfd;
            if (i > maxi)
                maxi  = i;
            if (--nready <= 0) {
                continue; // no more readable descriptors
            }
        }

        // handle each coming connection
        for (i = 0; i <= maxi; i++) {
            if ((sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset)) {
                if ((n = Read(sockfd, buff, MAXLINE)) == 0) {
                    // connection is closed by client.
                    
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else {
                    buff[n] = 0;
                    sprintf(buff2, "[server]:%s", buff);
                    Write(sockfd, buff2, strlen(buff2));
                }
                nready--;
                if (nready <= 0)
                    break;  // go to next Select()
            }
        }
    }
    
}

void server_poll()
{
    int i, maxi, listenfd, connfd, sockfd;

    int nready;

    // struct pollfd {
    // int fd;
    // int events;  // input
    // int revents;  // output
    // }
    // The OPEN_MAX indicates the max num of fd the current process can open.
    // The listenfd is stored at the first place of the client array
    struct pollfd client[OPEN_MAX];

    struct sockaddr_in cliaddr, srvaddr;
    socklen_t          clen;
    
    ssize_t n;

    char buff[MAXLINE];
    char buff2[MAXLINE + 10];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&srvaddr, sizeof(srvaddr));
    srvaddr.sin_family      = AF_INET;
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvaddr.sin_port        = htons(SERV_PORT);

    Bind(listenfd, (SA*)&srvaddr, sizeof(srvaddr));

    Listen(listenfd, LISTENQ);  // set max of listen quueue

    client[0].fd     = listenfd;
    client[0].events = POLLRDNORM;
    
    for (i = 1; i < OPEN_MAX; i++)
        client[i].fd = -1;      // -1 means this pollfd is ignored by poll()
    
    maxi = 0;  // the max index of valid follfd

    for (; ;) {
        nready = Poll(client, maxi + 1, INFTIM); // INFTIM means waiting for ever

        if (client[0].revents & POLLRDNORM) {
            // new client is comming...
            clen   = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA*)&cliaddr, &clen);

            // store the new connfd into client array
            for (i = 1; i < OPEN_MAX; i++) {
                if (client[i].fd < 0) {
                    client[i].fd = connfd;
                    break;
                }
            }
            if (i == OPEN_MAX)
                err_quit("too many clients");

            client[i].events = POLLRDNORM;
            
            if (i > maxi)
                maxi = i;

            nready --;
            if (nready <= 0)
                continue;
        }

        for (i = 1; i <= maxi; i++) {
            if ((sockfd = client[i].fd) < 0)
                continue;

            if (client[i].revents & (POLLRDNORM | POLLERR)) {
                if ((n = read(sockfd, buff, MAXLINE)) < 0) {
                    if (errno == ECONNRESET) {
                        // connection is reset by client
                        Close(sockfd);
                        client[i].fd = -1;
                    } else {
                        err_sys("read error.");
                    }
                } else if (n == 0) {
                    // the connection is closed by client
                    Close(connfd);
                    client[i].fd = -1;
                } else {
                    // n > 0
                    buff[n]      = 0;
                    sprintf(buff2, "[srv poll]:%s", buff);
                    Write(sockfd, buff2, strlen(buff2));
                }
                nready --;
                if (nready <= 0) {
                    break;  // go to Poll()
                }
            }
        }
    }

}

int main(int argc, char ** argv)
{
    if (argc == 2 && strcmp(argv[1], "poll") == 0) {
        server_poll();
    } else {
        server_select();
    }
    exit(0);
}

