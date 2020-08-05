#include "epoll_helper.h"
#include "tinyekf_config.h"

static double data_input[16];
static int fd[MAXPORT * 2][2]; //fd[0] read, fd[1] write
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
    	}
	close(infd);
	close(outfd);
	return 0;
}

int 
main(int argc, char *argv[]) {

	char   data_line[16][MAX];
	char   sout[3][MAX];
	struct sockaddr_in serverAddr, clientAddr;
    	serverAddr.sin_family = PF_INET;
    	serverAddr.sin_port = htons(SERVER_PORT);
    	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
        socklen_t cliLen = sizeof(clientAddr); 
	// map key : port, value: pid
    	//map<int, int> port_pid; 
	int port_pid[MAXPORT];
	// map key : fd, value: port
	int fd_port[MAXPORT];
	//map<int, int> fd_port; 
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

	//create epoll
    	int epfd = epoll_create(EPOLL_SIZE);
    	if(epfd < 0) { 
		perror("epfd error");
		exit(-1);
	}
   	printf("epoll created, epollfd = %d\n", epfd);
    	static struct epoll_event events[PIDNUMB * 2 + 1];
    	//sock add to epfd list
    	addfd(epfd, listener);
    	//main loop
   	while(1) {
		int i;
		int count = epoll_wait(epfd, events, PIDNUMB * 2, EPOLL_SIZE);
		printf("num of events %d\n", count);
		for (i = 0; i < count; ++i) {
			//printf("events[i].data.fd %d listener %d", events[i].data.fd, listener);
			if (events[i].data.fd == listener) {
				recvfrom(listener, data_line, sizeof(data_line), 0, ( struct sockaddr* )&clientAddr, &cliLen);
				int c_port = clientAddr.sin_port;  
				printf("client port %d \n", c_port);
				//if (!port_pid.count(c_port)) { //new client
				if (port_pid[c_port] == 0) { //new client
					int pid = fork();

					if (pipe(fd[pid]) < 0 || pipe(fd[pid * 2]) < 0) {
						perror("pipe error");
						exit(1);
					}
				        addfd(epfd, fd[pid * 2][0]); //listen to child
    					if(pid < 0) { 
						perror("fork error"); 
						exit(-1); 
					} else if (pid == 0) { // child
						printf("child process\n");
						handle_ekf(fd[pid][0], fd[pid * 2][1]);
					} else { //parent
						port_pid[c_port] = pid;
						fd_port[fd[pid * 2][0]] = c_port;
						//port_pid.insert(pair<int, int>(c_port, pid));
						//fd_port.insert(pair<int, int>(fd[pid * 2][0], c_port));
						write(fd[pid][1], data_line, sizeof(data_line));
					}	
				} else {
					//int pid = port_pid.find(clientAddr.sin_port)->second;
					int pid = port_pid[clientAddr.sin_port];
					write(fd[pid][1], data_line, sizeof(data_line));
				
				}
			} else if (events[i].events & EPOLLIN) {
				int outfd = events[i].data.fd;
				//int port = fd_port.find(outfd)->second;
				int port = fd_port[outfd];
				read(outfd, sout, sizeof(sout));
				clientAddr.sin_port = port;
				printf("sent to client\n");
				sendto(listener, sout, sizeof(sout), 0, ( struct sockaddr* )&clientAddr, &cliLen);
			}
		}
       	}
    	close(listener); //close socket
    	close(epfd); 
    	return 0;
}
