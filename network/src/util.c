#include "util.h"

void printSockName(int sockfd) {
    struct sockaddr_in addr;
    socklen_t          len = sizeof(addr);

    if (getsockname(sockfd, (SA*)&addr, &len) < 0) {
        printf("Failed to call getsockname()\n");
    } else {
        char buff[MAXLINE];
        printf("Sock address: %s, port: %d\n", inet_ntop(AF_INET, &addr.sin_addr, buff, sizeof(buff)), ntohs(addr.sin_port));
    }
}

void printPeerSockName(int sockfd) {
    struct sockaddr_in addr;
    socklen_t          len = sizeof(addr);

    if (getpeername(sockfd, (SA*)&addr, &len) < 0) {
        printf("Failed to call getpeername()\n");
    } else {
        char buff[MAXLINE];
        printf("Peer sock address: %s, port: %d\n", inet_ntop(AF_INET, &addr.sin_addr, buff, sizeof(buff)), ntohs(addr.sin_port));
    }
}

