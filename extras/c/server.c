#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>  
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <unistd.h> 
#define PORT 8080 
#define MAX  30
#define SA struct sockaddr   
static double data_input[2][16];
static int itr = 0;
extern double * ekf(double data_input[2][16], int itr);
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
    	servaddr.sin_addr.s_addr = INADDR_ANY;//htonl(INADDR_ANY); 
    	servaddr.sin_port = htons(PORT); 
  
    	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) < 0) { 
        	printf("socket bind failed...\n"); 
        	exit(0); 
    	} else {
        	printf("Socket successfully binded..\n"); 
	}	
  
    	len = sizeof(cli); 
	char   data_line[16][MAX];
	double * pos;
	char   sout[3][MAX];
	while (1) { 
		int i;
        	recvfrom(sockfd, data_line, sizeof(data_line), 0, (SA*)&cli, &len); 
		for (i = 0; i < 16; i++) {
			data_input[itr % 2][i] = atof(data_line[i]);
			printf("%.16f",data_input[itr % 2][i]);			
		}
		pos = ekf(data_input, itr % 2);
		for (i = 0; i < 3; i++) {
			sprintf(sout[i],"%.16f", pos[i]);
		} 
		sendto(sockfd, sout, sizeof(sout), 0, (SA*)&cli, len);
		printf("\n\n");
        	itr++; 
    	}
    	close(sockfd); 
	return 0;
} 

