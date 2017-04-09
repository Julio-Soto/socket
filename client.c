#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>

#define PORT 6869
#define MAXDATASIZE 100

using namespace std;

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	char sendbuf[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in their_addr;
	char username[MAXDATASIZE];
	char password[MAXDATASIZE];
	char msg[MAXDATASIZE];
	bool loggedin = 0;



	if (argc !=2)
	{
		fprintf(stderr,"usage: client hostname\n");
		exit(1);
	}

	if((he=gethostbyname(argv[1])) == NULL){
		perror("gethostbyname");
		exit(1);
	}
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}

	bzero((char *) &their_addr, sizeof(their_addr));
	their_addr.sin_family = AF_INET;
	bcopy((char *)he->h_addr,(char *)&their_addr.sin_addr.s_addr,he->h_length);
	//memset(&(their_addr.sin_zero),'\0',8);
	their_addr.sin_port = htons(PORT);

	if(connect(sockfd,(struct sockaddr *) &their_addr, sizeof(their_addr)) == -1){
		perror("no connect!!");
		exit(1);
	}

	printf("Welcome to the eShedule Server!!!!\n");

	while(!loggedin){
		cout << "login as: ";
        	cin >> username;

		strcpy(sendbuf,username);
		if((numbytes=send(sockfd, sendbuf, sizeof(sendbuf),0)) == -1){
	  		perror("send");
	  		close(sockfd);
	  		exit(1);
	  	}

	  	if((numbytes=recv(sockfd, buf, 127, 0)) == -1){
	  		perror("recv");
	  		exit(1);
	  	}


		cout << "password: ";
		cin >> password;

		strcpy(sendbuf,password);
		if((numbytes=send(sockfd, sendbuf, sizeof(sendbuf),0)) == -1){
	  		perror("send");
	  		close(sockfd);
	  		exit(1);
	  	}

	  	if((numbytes=recv(sockfd, buf, 127, 0)) == -1){
	  		perror("recv");
	  		exit(1);
	  	}

	  	cin.clear();
		fflush(stdin);
		strcpy(sendbuf,"");
		loggedin = true;
	}

	for(;;){
		cout << "enter a command: ";
		cin >> msg;
		strcpy(sendbuf,msg);
		numbytes = sizeof(sendbuf);
		sendbuf[numbytes] = '\0';

	 if(numbytes == 0 || strncmp(sendbuf,"bye",3) == 0){
	 	printf("bye\n");
	 	break;

	 }//endif
	  else{
	  	if((numbytes=send(sockfd, sendbuf, sizeof(sendbuf),0)) == -1){
	  		perror("send");
	  		close(sockfd);
	  		exit(1);
	  	}
	  	sendbuf[numbytes] = '\0';
	  	printf("sent: %s\n",sendbuf);

	  	if((numbytes=recv(sockfd, buf, 127, 0)) == -1){
	  		perror("recv");
	  		exit(1);
	  	}

	  	buf[numbytes] = '\0';

	  	printf("received: %s\n",buf);
	  }//endelse

	}//endfor
	close(sockfd);

	return 0;
}

