#include "unp.h"

void str_echo(int sockfd) {
    ssize_t n;
    char    buff[MAXLINE];

    while (1) {
        while((n = read(sockfd, buff, MAXLINE)) > 0) {  // read from client
            Writen(sockfd, buff, n);   // write to client
        }

        if (n < 0 && errno != EINTR) {
            err_sys("str_echo: read error.");
        } else {  // n == 0
            break;
        }
    }
}

void sig_child(int signo) {
    // wait for child process exiting. otherwise the child process would become zombie
    // call "ps -o pid,ppid,tty,stat,args" to display process detials.
    pid_t childpid;
    int   stat;

    // v1
    //childpid = wait(&stat);
    //printf("child process %d terminated\n", childpid);

    //v2
    // -1 means the parent process waits on first terminated child process
    // WNOHANG means the system does not hang the parent proces if no child is terminated.
    while ((childpid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("Child process %d terminated.\n", childpid);
    }
    
    return;
}

int main(int argc, char ** argv)
{
    int                listenfd, connfd;
    pid_t              childpid;
    socklen_t          chilen;
    struct sockaddr_in cliaddr, srvaddr;

    // 1, create socket (IPv4, TCP/stream)
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    // 2, bind to any address
    bzero(&srvaddr, sizeof(srvaddr));
    srvaddr.sin_family      = AF_INET;
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvaddr.sin_port        = htons(SERV_PORT);
    Bind(listenfd, (SA*)&srvaddr, sizeof(srvaddr));

    // 3, Listen
    Listen(listenfd, LISTENQ);

    // 4, register SIGCHLD signal handler
    Signal(SIGCHLD, sig_child);
    
    // 5, Accept and fork
    for (; ;) {
        chilen = sizeof(cliaddr);
        connfd = accept(listenfd, (SA*)&cliaddr, &chilen);  // the wrapper is Accept()

        if (connfd < 0) {
            if (errno == EINTR) {
                // If system Interrupt happens, we should restore the accept call.
                continue;
            } else {
                err_sys("Accept error");
            }
        }
        
        if ((childpid = Fork()) == 0) {
            // child process which duplidate resources from parent process
            // when the child process is exiting, it would send SIGCHLD signal. the parent shouild handle this signal
            // if the parent does not handle the SIGCHLD, this child process becomes zombie.
            Close(listenfd);
            str_echo(connfd);
            Close(connfd);
            exit(0);
        }

        Close(connfd);   // Important to close the connection within the parent process!
    }

    exit(0);
}

