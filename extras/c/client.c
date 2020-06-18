#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#define PORT 8080 
#define SA struct sockaddr 
void 
func(int sockfd) 
{ 
	double cout[3]={0.1,0.1,0.1};  
	char   c[3][26];
	char   s[3][26];
	double sout[3];
	for (;;) { 
		printf("send the double array :\n "); 
		int i;
		for (i = 0; i < 3; i++) {
			sprintf(c[i],"%.16f",cout[i]);
		} 
		write(sockfd, c, sizeof(c)); 
		read(sockfd, s, sizeof(s)); 
		printf("From Server received\n");
		for (i = 0; i < 3; i++) {
			sout[i] = atof(s[i]);
			printf("%.16f",sout[i]);			
		}
	} 
} 

int 
main() 
{ 
	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 

	// socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server..\n"); 

	// function for chat 
	func(sockfd); 

	// close the socket 
	close(sockfd); 
} 

