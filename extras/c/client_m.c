#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <pthread.h>
#include <gpsdata.h>
#include "epoll_helper.h"
#define MAX  32
#define PORT 8888 
#define SA struct sockaddr 

void*
send_rq(int sockfd)
{
	int itr = 0;
	char   data_line[16][MAX]; 
	char   pos_line[3][MAX]; //x,y,z
	double pos[3];
	while (1) { 
		int i;
		for (i = 0; i < 16; i++) {
			sprintf(data_line[i],"%.16f", input[itr % 25][i]);
		} 
		printf("sent\n");
		sendto(sockfd, data_line, sizeof(data_line), 0, (SA*) &servaddr, sizeof(servaddr)); 
		int len;
		len = sizeof(servaddr);
		recvfrom(sockfd, pos_line, sizeof(pos_line), 0, (SA*) &servaddr, &len); 
		printf("From Server received position\n");
		for (i = 0; i < 3; i++) {
			pos[i] = atof(pos_line[i]);			
		}
		printf("\n");
		itr++;
	} 

}
 
int 
main() 
{ 
	int sockfd, i, ret; 
	pthread_t pID;

	struct sockaddr_in servaddr; 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	if (sockfd < 0) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Socket successfully created..\n"); 
	}
	memset(&servaddr, 0, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT); 
	setnonblocking(sockfd);

	ret = pthread_creat(&pID, NULL, send_rq, );
	pthread_join(pID, NULL);
	close(sockfd); 
	return 0;
} 

