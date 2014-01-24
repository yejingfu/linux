#include "unp.h"
#include "util.h"


int main(int argc, char **argv)
{

  // test size of size_t(8 bytes) and socklen_t(4 bytes)
  //  printf("size_t: %lu, socklen_t: %lu\n", sizeof(size_t), sizeof(socklen_t));
    
  int                sockfd, n;
  char               recvline[MAXLINE+1];
  struct sockaddr_in servaddr;

  if (argc != 2)
    err_quit("usage: 10client <IPAddress>");

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err_sys("socket error");

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family                                  = AF_INET;
  servaddr.sin_port                                    = htons(13);
  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    err_quit("inet_pton error for %s", argv[1]);

  if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
    printf("errno: %d\n", errno);
    err_sys("connet error");
  }

  printSockName(sockfd);
  printPeerSockName(sockfd);

  int totalCount = 0;
  
  while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
    totalCount  += n;
    recvline[n]  = 0;
    if (fputs(recvline, stdout) == EOF)
      err_sys("fputs error");
  }

  if (n < 0)
      err_sys("read error");
  
  printf("total: %d\n", totalCount);
  
  exit(0);
}

