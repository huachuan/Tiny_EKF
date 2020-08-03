#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>  
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/resource.h>
#include "tinyekf_config.h"
#define MAX  32
#define BACKLOG 5
#define PORT 8080
#define PIDNUMB 100000
#define SA struct sockaddr  
#define MAXEPOLLSIZE 100 
static double data_input[16];
static ekf_t ekf;
extern void init(ekf_t * ekf);
extern double * ekf_fn(ekf_t * ekf, double data_input[16]);
extern void ekf_init(ekf_t * ekf, int n, int m);

void
handle_connect(int sockfd)
{
/*	int len, byte_read; 
    	struct sockaddr_in cli; 	
	len = sizeof(cli); */
	char   data_line[16][MAX];
	double * pos;
	char   sout[3][MAX];
	while (1) { 
		int i;
		byte_read = 0;
        	byte_read = recvfrom(sockfd, data_line, sizeof(data_line), 0, (SA *)&cli, &len); 
		for (i = 0; i < 16; i++) {
			data_input[i] = atof(data_line[i]);			
		}
		pos = ekf_fn(&ekf, data_input);
		for (i = 0; i < 3; i++) {
			sprintf(sout[i],"%f", pos[i]);
		} 
		sendto(sockfd, sout, sizeof(sout), 0, (SA *)&cli, len);	
		printf("sockfd %d client port %d and pid %d", sockfd, cli.sin_port, getpid());	
		printf("\n\n");
    	}
	close(sockfd);
}
void
handle_data()
{
	char   data_line[16][MAX];
	double * pos;
	char   sout[3][MAX];
	for (i = 0; i < 16; i++) {
		data_input[i] = atof(data_line[i]);			
	}
	pos = ekf_fn(&ekf, data_input);
	for (i = 0; i < 3; i++) {
		sprintf(sout[i],"%f", pos[i]);
	} 	
	printf("\n\n");
    	}
}
int 
setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1){
    		return -1;
  	}
	return 0;
}

int 
main() 
{ 
	int sockfd, len, kdpfd, i, client; 
	struct sockaddr_in servaddr, cli; 
	int shmid;
	int *shmaddr;
	struct shmid_ds buf;
	shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT|0600);
	if (shmid < 0) {
		perror("new shm error");
		return -1;	
	}
  	/*
  	struct epoll_event ev;
  	struct epoll_event events[MAXEPOLLSIZE];
  	struct rlimit rt;
 
  	rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
  	if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
    		perror("setrlimit");
    		exit(1);
  	} else {
    		printf("setting success /n");
 	}
	*/
    	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    	if (sockfd < 0) { 
        	printf("socket creation failed...\n"); 
        	exit(0); 
    	} else {
        	printf("Socket successfully created..\n"); 
	}
 /*
	int opt=SO_REUSEADDR;
  	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
*/
	setnonblocking(sockfd);
    	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cli, 0, sizeof(cli)); 
  
    	servaddr.sin_family = AF_INET; 
    	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    	servaddr.sin_port = htons(PORT); 
        
    	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) < 0) { 
        	printf("socket bind failed...\n"); 
        	exit(0); 
    	} else {
        	printf("Socket successfully binded..\n"); 
	}
/*create epoll
  	kdpfd = epoll_create(MAXEPOLLSIZE);
  	len = sizeof(struct sockaddr_in);
 	ev.events = EPOLLIN | EPOLLET;
  	ev.data.fd = sockfd;
  	if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
    		fprintf(stderr, "epoll set insertion error: fd=%d/n", sockfd);
    		return -1;
  	} else {
    		printf("listen socket added in  epoll success /n");
  	}
/*wait*/
	int len, byte_read; 
    	struct sockaddr_in cli; 	
	len = sizeof(cli);
	/*init ekf here*/
	ekf_init(&ekf, Nsta, Mobs);
	init(&ekf);
	int   pid_port[PIDNUMB];
	pid_t pid[PIDNUMB]; 
	char   data_line[16][MAX];
	while (1) {
/*
    		nfds = epoll_wait(kdpfd, events, 10000, -1);
    		if (nfds == -1) {
      			perror("epoll_wait");
      			break;
   		}
    		for (i = 0; i < nfds; i++) {
      			if (events[i].data.fd == sockfd) {
				        client = accept(sockfd, (struct sockaddr *) &servaddr, &len);
        				if (client < 0){
            					perror("accept");
            					continue;
       					}
*/	
		recvfrom(sockfd, data_line, sizeof(data_line), 0, (SA *)&cli, &len);
		if (pid_port[cli.sin_port] == 0) {
			pid[cli.sin_port] = fork();
			if (pid[cli.sin_port] == 0) { 
				pid_port[cli.sin_port] = 1;
				shmaddr = (int *)shmat(shmid, NULL, 0);
			        if ((int)shmaddr == -1) {
					perror("shmat error");
					return -1;				
				}
				handle_data();
				/*TODO create a queue*/	
			}
		} 
		/*TODO*/
		/*add to data_line to queue*/
		
	/*init ekf here
	ekf_init(&ekf, Nsta, Mobs);
	init(&ekf);/*
	char buff[MAX];
	int  i_cli = 0, new_port;
	int  byte_read;
	len = sizeof(cli);
	while(1) {
		printf("UDP server: waiting for connection\n");
		bzero(buff, MAX);
		byte_read = recvfrom(sockfd, (char *)buff, MAX, MSG_WAITALL, (SA *)&cli, &len);
		buff[byte_read] = '\0';
		printf("%s \n", buff);
		if (byte_read > 0) {
			i_cli++;
			new_port = PORT + i_cli;
			printf("received new, assign a port to client\n");
			bzero(buff, MAX);
			sprintf(buff, "%d", new_port);
			printf("port number %s\n", buff);
			sendto(sockfd, (char *)buff, sizeof(buff), 0, (SA *)&cli, len);
			struct sockaddr_in childsaddr; 
			int childfd = socket(AF_INET, SOCK_DGRAM, 0); 
	    		if (childfd < 0) { 
				printf("child socket creation failed...\n"); 
				exit(0); 
	    		} else {
				printf("child socket successfully created..\n"); 
			}
			memset(&childsaddr, 0, sizeof(childsaddr)); 
	    		childsaddr.sin_family = AF_INET; 
	    		childsaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	    		childsaddr.sin_port = htons(new_port); 
			pid[i_cli] = fork();
			if (pid[i_cli] == 0) { /*
				struct sockaddr_in childsaddr; 
				int childfd = socket(AF_INET, SOCK_DGRAM, 0); 
	    			if (childfd < 0) { 
					printf("child socket creation failed...\n"); 
					exit(0); 
	    			} else {
					printf("child socket successfully created..\n"); 
				}
				memset(&childsaddr, 0, sizeof(childsaddr)); 
	    			childsaddr.sin_family = AF_INET; 
	    			childsaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	    			childsaddr.sin_port = htons(new_port); 
	    			if ((bind(childfd, (SA*)&childsaddr, sizeof(childsaddr))) < 0) { 
					printf("socket bind failed...\n"); 
					exit(0); 
	    			} else {
					printf("Socket successfully binded for client #%d\n", i_cli); 
				}
	    			if ((bind(childfd, (SA*)&childsaddr, sizeof(childsaddr))) < 0) { 
					printf("socket bind failed...\n"); 
					exit(0); 
	    			} else {
					printf("Socket successfully binded for client #%d\n", i_cli); 
				}
				printf("UDP server: receive use new port\n");
				handle_connect(childfd);
			}	
		}
	}*/
	close(sockfd);	
	return 0;
} 

