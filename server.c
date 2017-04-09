/* A simple server in the internet domain using TCP */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MYPORT 6869
#define BACKLOG 10

void sigchld_handler(int s){
	while(wait(NULL) > 0);
}

int main(void){
	int sockfd, new_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	unsigned int sin_size;
	struct sigaction sa;
	int yes  = 1;
	char *recvbuf;
	char *caddr;
	int numbytes;
	char username[20];
	char password[20];
	bool auth = 0;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}

	printf("SOCK_FD=%d/n", sockfd);
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYPORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero),'\0',8);

	if(bind(sockfd, (struct sockaddr *)&my_addr,sizeof(struct sockaddr)) == -1 ){
		perror("bind");
		exit(1);
	}

	if(listen(sockfd,BACKLOG) == -1){
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD,&sa, NULL) == -1){
		perror("sigaction");
		exit(1);
	}

	while(1){//main accept loop
		sin_size = sizeof(struct sockaddr_in);
		if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1 ){
			perror("accept");
			continue;
		}
		printf("server: got connection from %s\n", (char *) inet_ntoa(their_addr.sin_addr));
		if(!fork()) {
			close(sockfd);
			recvbuf = (char *) calloc(128,sizeof(char));

			while(!auth){
				numbytes = recv(new_fd,recvbuf,128,0);
				if(numbytes < 0){
					perror("recv");
					close(new_fd);
					exit(1);
				}
				else if(numbytes == 0 || strncmp(recvbuf,"bye",3) == 0) {
					printf("client(%s) has been disconnected \n", (char *) inet_ntoa(their_addr.sin_addr));
					close(new_fd);
					exit(0);	
				}

				printf("Received from %s: %s\n",inet_ntoa(their_addr.sin_addr),recvbuf);
				if(send(new_fd, recvbuf, numbytes, 0) == -1){
					perror("send");
					close(new_fd);
					exit(1);					
				}
				strcpy(username,recvbuf);
				printf("user: %s\n",username);
				if(strcmp(username, "myuser") == 0){
					auth = 1;
					printf("gained entry \n");
				}
			}
			

			for(;;){
				printf("in main accept loop \n");
				numbytes = recv(new_fd,recvbuf,128,0);
				if(numbytes < 0){
					perror("recv");
					close(new_fd);
					exit(1);
				}
				else if(numbytes == 0 || strncmp(recvbuf,"bye",3) == 0) {
					printf("client(%s) has been disconnected \n", (char *) inet_ntoa(their_addr.sin_addr));
					close(new_fd);
					exit(0);	
				}

				printf("Received from %s: %s\n",inet_ntoa(their_addr.sin_addr),recvbuf);
				if(send(new_fd, recvbuf, numbytes, 0) == -1){
					perror("send");
					close(new_fd);
					exit(1);					
				}


			}
			close(new_fd);
			exit(0);
		}
		close(new_fd);
	}
	return 0;
}



















