#include "unp.h"
#include "util.h"
#include <time.h>

int main(int argc, char ** argv)
{
  int                listenfd, connfd;
  struct sockaddr_in servaddr;
  struct sockaddr_in clientaddr;
  char               buff[MAXLINE];
  time_t             ticks;

  // The port 0~1023 is reserved by system.
  // If the server is listening on the reserved port, you should run server by sudo.
  // If the port is between 1024~65535, no need to run the server by sudo.
  
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(13);

  Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
  printSockName(listenfd);

  printf("The server is listening the the port 13....\n");

  Listen(listenfd, LISTENQ);

  for (; ;) {

      // If you don't care the client address, you can just pass NULL to the second and third parameters.
      socklen_t len = sizeof(clientaddr);
      connfd        = Accept(listenfd, (SA*)&clientaddr, &len);
      printf("Connection from %s, port %d\n", inet_ntop(AF_INET, &clientaddr.sin_addr, buff, sizeof(buff)), ntohs(clientaddr.sin_port));
      printSockName(connfd);
      printPeerSockName(connfd);
      
    //    connfd    = Accept(listenfd, (SA*)NULL, NULL);      
    ticks  = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    // 1, write the whole buffer one time.
    Write(connfd, buff, strlen(buff));
    
    // 2, write each char per time
    /*
    int len = strlen(buff);
    printf("len: %d\n", len);
    for (int i = 0; i < len; i++) {
        Write(connfd, buff + i, 1);
    }
    */

    Close(connfd);
  }
}

