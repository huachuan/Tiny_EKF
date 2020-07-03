#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>  
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include "tinyekf_config.h"
#define MAX  32
#define BACKLOG 5
#define PORT 8080
#define PIDNUMB 2
#define SA struct sockaddr   
static double data_input[16];
static ekf_t ekf;
extern void init(ekf_t * ekf);
extern double * ekf_fn(ekf_t * ekf, double data_input[16]);
extern void ekf_init(ekf_t * ekf, int n, int m);
void
sig_int(int num) 
{
	exit(1);
}
static void
handle_connect(int sockfd)
{
	int len; 
    	struct sockaddr_in cli; 	
	len = sizeof(cli); 
	char   data_line[16][MAX];
	double * pos;
	char   sout[3][MAX];
	ekf_init(&ekf, Nsta, Mobs);
	init(&ekf);
	while (1) { 
		int i;
        	recvfrom(sockfd, data_line, sizeof(data_line), 0, (SA *)&cli, &len); 
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
int 
main() 
{ 
	int sockfd; 
	struct sockaddr_in servaddr, cli; 
	//signal(SIGINT, sig_int);
    	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    	if (sockfd < 0) { 
        	printf("socket creation failed...\n"); 
        	exit(0); 
    	} else {
        	printf("Socket successfully created..\n"); 
	}

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
	
  	pid_t pid[PIDNUMB];
	int   i = 0;
	for (i = 0; i < PIDNUMB; i++) {
		pid[i] = fork();
		if (pid[i] == 0) {
			handle_connect(sockfd);	
		}	
	}
 	while(1);
	return 0;
} 

