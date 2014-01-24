#include "unp.h"

void str_cli(FILE *fp, int sockfd) {
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    // Ctl-D to send EOF (EOF == NULL)
    while (Fgets(sendline, MAXLINE, fp) != NULL) { // read from file

        // v1
        Writen(sockfd, sendline, strlen(sendline)); // write to server

        // v2: SIGPIPE
        //Write(sockfd, sendline, 1);
        //sleep(1);
        //Write(sockfd, sendline+1, strlen(sendline)-1);

        if (Readline(sockfd, recvline, MAXLINE) == 0) { // read one line from server
            err_quit("str_cli: server terminated prematurely.");
        }

        Fputs(recvline, stdout);  // Print to stdout
    }
}

int main(int argc, char ** argv)
{
    // Read text from stdin, and send to server. Then read it from server and print to stdout.
    if (argc != 2) {
        err_quit("Usage: a5echo_client.out <IP>");
    }

    int                sockfd;
    struct sockaddr_in servaddr;

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    Connect(sockfd, (SA*)&servaddr, sizeof(servaddr));
    str_cli(stdin, sockfd);

    exit(0);
}

