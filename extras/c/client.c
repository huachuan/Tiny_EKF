#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h> 
#include <gpsdata.h>
#define MAX  30
#define PORT 8080 
#define SA struct sockaddr 
static int itr = 0;
static double pos[25][3];
/*
void 
func(int sockfd) 
{  
	char   data_line[16][MAX]; 
	char   pos_line[3][MAX]; //x,y,z
	for (;;) { 
		//send data to server
		int i;
		for (i = 0; i < 16; i++) {
			sprintf(data_line[i],"%.16f", input[itr % 25][i]);
		} 
		sendto(sockfd, data_line, sizeof(data_line), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
		recvfrom(sockfd, pos_line, sizeof(pos_line)); 
		printf("From Server received\n");
		for (i = 0; i < 3; i++) {
			pos[itr % 25][i] = atof(pos_line[i]);
			printf("%.16f",pos[itr % 25][i]);			
		}
		printf("\n");
		itr++;
	} 
} */
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
	/*func(sockfd); */
	char   data_line[16][MAX]; 
	char   pos_line[3][MAX]; //x,y,z
	while (1) { 
		//send data to server
		int i;
		for (i = 0; i < 16; i++) {
			sprintf(data_line[i],"%.16f", input[itr % 25][i]);
		} 
		sendto(sockfd, data_line, sizeof(data_line), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
		int len;
		len = sizeof(servaddr);
		recvfrom(sockfd, pos_line, sizeof(pos_line), 0, (struct sockaddr *) &servaddr, &len); 
		printf("From Server received\n");
		for (i = 0; i < 3; i++) {
			pos[itr % 25][i] = atof(pos_line[i]);
			printf("%.16f",pos[itr % 25][i]);			
		}
		printf("\n");
		itr++;
	} 

	// close the socket 
	close(sockfd); 
} 

