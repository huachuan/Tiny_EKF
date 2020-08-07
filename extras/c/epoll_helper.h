#ifndef EPOLL_HELPER_H_INCLUDED
#define EPOLL_HELPER_H_INCLUDED
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
 
/**********************   macro defintion **************************/
// server ip
//#define SERVER_IP "127.0.0.1"
 
// server port
#define SERVER_PORT 8888
 
//epoll size
#define EPOLL_SIZE 5000
// exit
#define EXIT "EXIT"
 
#define CAUTION "There is only one int the char room!"

#define PIDNUMB 50000
#define MAX     64 
#define MAXPORT 50000
/**********************   some function **************************/
/**
  * @param sockfd: socket descriptor
  * @return 0
**/
int setnonblocking(int sockfd)
{
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)| O_NONBLOCK);
    return 0;
}
 
/**
 epollfd: epoll handle
 socket descriptor
 enable_et : enable_et = true, epoll use ET; otherwise LT
**/
void addfd( int epollfd, int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setnonblocking(fd);
    printf("fd added to epoll!\n\n");
}
#endif // UTILITY_H_INCLUDED
