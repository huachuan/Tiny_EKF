#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>  
#define PORT 8080 
#define MAX  30
#define SA struct sockaddr   
// Function designed for chat between client and server. 
static double data_input[25][16];
static int itr = 0;
extern double * ekf(double data_input[25][16], int itr);
void 
func(int sockfd) 
{  
	char   data_line[16][MAX];
	double * pos;
	char   sout[3][MAX];
	for (;;) { 
		int i;
        	read(sockfd, data_line, sizeof(data_line)); 
		for (i = 0; i < 16; i++) {
			data_input[itr % 25][i] = atof(data_line[i]);
			printf("%.16f",data_input[itr % 25][i]);			
		}
		pos = ekf(data_input, itr % 25);
		for (i = 0; i < 3; i++) {
			sprintf(sout[i],"%.16f", pos[i]);
		} 
		write(sockfd, sout, sizeof(sout));
		printf("\n\n");
        	itr++; 
    	}
} 
  
// Driver function 
int 
main() 
{ 
	int sockfd, connfd, len; 
    	struct sockaddr_in servaddr, cli; 
  
    	// socket create and verification 
    	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    	if (sockfd == -1) { 
        	printf("socket creation failed...\n"); 
        	exit(0); 
    	} else {
        	printf("Socket successfully created..\n"); 
	}
    	bzero(&servaddr, sizeof(servaddr)); 
  
    	// assign IP, PORT 
    	servaddr.sin_family = AF_INET; 
    	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    	servaddr.sin_port = htons(PORT); 
  
    	// Binding newly created socket to given IP and verification 
    	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        	printf("socket bind failed...\n"); 
        	exit(0); 
    	} else {
        	printf("Socket successfully binded..\n"); 
	}	
  
    	// Now server is ready to listen and verification 
   	if ((listen(sockfd, 5)) != 0) { 
       		printf("Listen failed...\n"); 
        	exit(0); 
    	} else {
        	printf("Server listening..\n");
	}
    	len = sizeof(cli); 
  
    	// Accept the data packet from client and verification 
    	connfd = accept(sockfd, (SA*)&cli, &len); 
    	if (connfd < 0) { 
        	printf("server acccept failed...\n"); 
        	exit(0); 
    	} else {
        	printf("server acccept the client...\n"); 
	}
  	// Function for chatting between client and server 
    	func(connfd); 
  	//After chatting close the socket 
    	close(sockfd); 
	return 0;
} 

