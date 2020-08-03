#include "epoll_helper.h"
#include <map> 
#include "tinyekf_config.h"
using namespace std;
static double data_input[16];
static int fd[PIDNUMB * 2][2]; //fd[0] read, fd[1] write
static ekf_t ekf;
extern void init(ekf_t * ekf);
extern double * ekf_fn(ekf_t * ekf, double data_input[16]);
extern void ekf_init(ekf_t * ekf, int n, int m);

int
handle_ekf(int infd, int outfd)
{

	char   data_line[16][MAX];
	double * pos;
	char   sout[3][MAX];
	while (1) { 
		int i;
        	read(infd, data_line, sizeof(data_line)); 
		for (i = 0; i < 16; i++) {
			data_input[i] = atof(data_line[i]);			
		}
		pos = ekf_fn(&ekf, data_input);
		for (i = 0; i < 3; i++) {
			sprintf(sout[i],"%f", pos[i]);
		} 
		write(outfd, sout, sizeof(sout));	
		printf("sockfd %d client port %d and pid %d", sockfd, cli.sin_port, getpid());	
		printf("\n\n");
    	}
	close(infd);
	close(outfd);
	return 0;
}

int 
main(int argc, char *argv[]) {

	char   data_line[16][MAX];

	struct sockaddr_in serverAddr, clientAddr;
    	serverAddr.sin_family = PF_INET;
    	serverAddr.sin_port = htons(SERVER_PORT);
    	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	
        socklen_t clientAddrLen = sizeof(struct sockaddr_in); 
	// map key : port, value: pid
    	map<int, int> port_pid; 
	// map key : fd, value: port
	map<int, int> fd_port; 
	pid_t pid[PIDNUMB];

    	int listener = socket(AF_INET, SOCK_DGRAM, 0);

    	if(listener < 0) {
		perror("listener"); 
		exit(-1);
	}
	printf("listen socket created \n");
    
    	if(bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        	perror("bind error");
        	exit(-1);
    	}

	//create epoll for pipe
    	int epfd = epoll_create(EPOLL_SIZE);
    	if(epfd < 0) { 
		perror("epfd error");
		exit(-1);
	}
   	printf("epoll created, epollfd = %d\n", epfd);
    	static struct epoll_event events[PIDNUMB * 2 + 1];
    	//sock add to epfd list
    	addfd(epfd, listener, true);
    	//main loop
   	while(1) {
		int i;
		nfds = epoll_wait(epfd, events, 20, 500);
		for (i = 0; i < nfds; ++i) {
			if (events[i].data.fd == listenfd) {
				recvfrom(listener, data_line, sizeof(data_line), 0, ( struct sockaddr* )&clientAddr, &clientAddrLen);
				int c_port = client_address.sin_port;  
				if (!port_pid.count(c_port)) {
					int pid = fork();

					if (pipe(fd[pid]) < 0) { //parent to child
						exit(1);
					}
					if (pipe(fd[pid * 2]) < 0) {// child to parent
						exit(1);
					}
				        addfd(epfd, fd[pid * 2][0], true); //listen to child
    					if(pid < 0 || pid > PIDNUMB) { 
						perror("fork error || pid > 1024"); 
						exit(-1); 
					} else if (pid == 0) { // child
						handle_ekf(fd[pid][0], fd[pid * 2][1]);
					} else { //parent
						port_pid.insert(pair<int, int>(c_port, pid));
						fd_port.insert(pair<int, int>(fd[pid * 2][0], c_port));
						write(fd[pid][1], data_line, sizeof(data_line));
					}
					
						
				} else {
				
				}
			} else (fd out /////)
		}
		

		if (!port_pid.count(c_port)) { //new client 
			port_pid.insert(pair<int, int>(c_port, pid_itr++));
    			if(pipe(pipe_fd) < 0) { 
				perror("pipe error"); 
				exit(-1); 
			}
 			int pid = fork();
    			if(pid < 0) { 
				perror("fork error"); 
				exit(-1); 
			} else if (pid == 0) {     // child process
        			//child process read from pipe，close write pipe
        			close(pipe_fd[c_pid][1]);
				    	//create epoll
    				
    				addfd(epfd, pipe_fd[0], true);
			} else { // parent process

				
			}	
            	} else {
			int pid = port_pid.find(client_address.sin_port)->second;
			write(pipe_fd[pid][1], data_line, sizeof(data_line));
            	}
		
       	}
    	close(listener); //close socket
    	close(epfd); 
    	return 0;
}
