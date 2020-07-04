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
#define PIDNUMB 80
#define SA struct sockaddr   
static double data_input[16];
static ekf_t ekf;
extern void init(ekf_t * ekf);
extern double * ekf_fn(ekf_t * ekf, double data_input[16]);
extern void ekf_init(ekf_t * ekf, int n, int m);

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
	int sockfd, len; 
	struct sockaddr_in servaddr, cli; 
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
			printf("received new, please sign a port to client\n");
			bzero(buff, MAX);
			sprintf(buff, "%d", new_port);
			printf("port number %s\n", buff);
			sendto(sockfd, (char *)buff, sizeof(buff), 0, (SA *)&cli, len);
			pid[i_cli] = fork();
			if (pid[i_cli] == 0) {
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
				//handle_connect(childfd);	
				printf("UDP server: receive use new port\n");
				len = sizeof(cli); 
				char   data_line[16][MAX];
				double * pos;
				char   sout[3][MAX];
				ekf_init(&ekf, Nsta, Mobs);
				init(&ekf);
					//while (1) { 
				int i;
				byte_read = recvfrom(childfd, data_line, sizeof(data_line), MSG_WAITALL, (SA *)&cli, &len); 
				printf("received byte_read = %d \n", byte_read);
				for (i = 0; i < 16; i++) {
					data_input[i] = atof(data_line[i]);			
				}
				pos = ekf_fn(&ekf, data_input);
				for (i = 0; i < 3; i++) {
					sprintf(sout[i],"%f", pos[i]);
				} 
				sendto(childfd, sout, sizeof(sout), 0, (SA *)&cli, len);
				printf("\n\n");
    	//}
			}	
		}
	}
	close(sockfd);	
	return 0;
} 

