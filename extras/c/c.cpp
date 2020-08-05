#include "epoll_helper.h"
 
int 
main(int argc, char *argv[]) {
    	//IP + port
    	struct sockaddr_in serverAddr;
    	serverAddr.sin_family = PF_INET;
    	serverAddr.sin_port = htons(SERVER_PORT);
    	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
 
    	// create socket
    	int sock = socket(AF_INET, SOCK_DGRAM, 0);
    	if(sock < 0) { 
		perror("sock error"); 
		exit(-1); 
	}
    	// connect to server
    	if(connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        	perror("connect error");
        	exit(-1);
    	}
 
    	// create pipe fd[0] parent read，fd[1] child write
    	int pipe_fd[2];
    	if(pipe(pipe_fd) < 0) { 
		perror("pipe error"); 
		exit(-1); 
	}
 
    	//create epoll
    	int epfd = epoll_create(EPOLL_SIZE);
    	if(epfd < 0) { 
		perror("epfd error");
		exit(-1);
	}
    	static struct epoll_event events[2];
    	//sock and pipe read port add to epfd list
    	addfd(epfd, sock, true);
    	addfd(epfd, pipe_fd[0], true);
    	// client works normal
    	bool isClientwork = true;
 
    	// buffer
    	char message[BUF_SIZE];
 
    	// Fork
    	int pid = fork();
    	if(pid < 0) { 
		perror("fork error"); 
		exit(-1); 
	} else if(pid == 0) {     // child process
        //child process write to pipe，close read pipe
        	close(pipe_fd[0]);
        	printf("Please input 'exit' to exit the chat room\n");
 
        	while(isClientwork){
            		bzero(&message, BUF_SIZE);
            		fgets(message, BUF_SIZE, stdin);
 
            // client exit
            		if(strncasecmp(message, EXIT, strlen(EXIT)) == 0){
                		isClientwork = 0;
            		} else {
            // child write to pipe
                		if( write(pipe_fd[1], message, strlen(message) - 1 ) < 0 ) { 
					perror("fork error"); exit(-1); 
				}
            		}
        	}
    	} else { //pid > 0 parent process
        //parent read from pipe, close write pipe
        	close(pipe_fd[1]);
 
        // main loop(epoll_wait)
        	while(isClientwork) {
            		int epoll_events_count = epoll_wait( epfd, events, 2, -1 );
            		for(int i = 0; i < epoll_events_count ; ++i) {
                		bzero(&message, BUF_SIZE);
 
                		//server message
               			if(events[i].data.fd == sock) {
                    //accept message
                    			int ret = recv(sock, message, BUF_SIZE, 0);
 
                    			// ret= 0 server closed
                    			if(ret == 0) {
                        			printf("Server closed connection: %d\n", sock);
                        			close(sock);
                        			isClientwork = 0;
                   			} else printf("%s\n", message);
 
                		} else {
                    //parent read from pipe
                    			int ret = read(events[i].data.fd, message, BUF_SIZE);
 
                    // ret = 0
                    			if (ret == 0) isClientwork = 0;
                    			else {   // sent to server
                      				send(sock, message, BUF_SIZE, 0);
                    			}
               			}
            		}//for
        	}//while
    	}
 
    	if(pid) {
       //close parent process and sock
        	close(pipe_fd[0]);
        	close(sock);
    	} else {
        // close child process
        	close(pipe_fd[1]);
    	}
    	return 0;
}
