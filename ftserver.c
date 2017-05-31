/*
** Morgan Brenner
** brennemo@oregonstate.edu 
** CS 372 Program 2 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>	

#define BUFFER_SIZE 1000
#define FILE_SIZE 10000

void catchSIGINT(int signo) {
	//foreground signal terminates self
		puts("\nForeground signal terminating.\n");				
		fflush(stdout);
		
		exit(0);
	}


void *get_in_addr(struct sockaddr *sa) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
}

struct addrinfo* fillAddrStruct(char *port) { 
    struct addrinfo hints, *servinfo; //*p;
    struct sigaction sa;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP	


    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    return servinfo;
}

int startup(struct addrinfo *servinfo) {
	int sockfd;  

	if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
		servinfo->ai_protocol)) == -1) {
		perror("server: socket");
	}

	//bind socket 
	if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		close(sockfd);
		perror("server: bind");
		exit(1);
	}

    return sockfd; 
}

int initiateOnDataConnection(char *host, char *port) {
	struct addrinfo hints, *res;
	char ipstr[INET_ADDRSTRLEN];
	
	int socketfd; 
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; 	
	hints.ai_socktype = SOCK_STREAM;		//TCP
	hints.ai_flags = AI_PASSIVE; 
	
	//fill addrinfo struct 
	if (getaddrinfo(host, port, &hints, &res) != 0) {
		fprintf(stderr,"error: getaddrinfo\n"); exit(1); 	
	}
	
	//create socket 
	socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socketfd == -1) { fprintf(stderr,"error: socket\n"); exit(1); }
	
	//establish connection 
	//status = connect(socketfd, res->ai_addr, res->ai_addrlen);
	if (connect(socketfd, res->ai_addr, res->ai_addrlen) == -1) {
		fprintf(stderr,"error: connect\n"); 
		close(socketfd);
		exit(1); 
	}
		
	freeaddrinfo(res);						//free linked list
	
	return socketfd;
	
}

void sendFileNames(char *host, char* port) {
	int q_fd;						//data connection file descriptor 
	char fileNames[BUFFER_SIZE];
	DIR *dp;
	struct dirent *ep;

	memset(fileNames, '\0', sizeof fileNames);

	dp = opendir("./");
	if (dp != NULL)
	{
		while (ep = readdir(dp)) {
			strcat(fileNames, ep->d_name);
			strcat(fileNames, "\n");
		}
		(void)closedir(dp);
	}
	else {
		perror("Couldn't read the directory");
		exit(1);
	}
	printf("%s", fileNames);

	q_fd = initiateOnDataConnection(host, port);

	//test sending a short string without loop
	send(q_fd, fileNames, strlen(fileNames), 0); 

	/*
	int sentLength = 0;
	int charsRead = 0;
	while (sentLength <= strlen(fileNames)) {
		charsRead = send(fd, fileNames, strlen(fileNames), 0); 
		sentLength += charsRead;
	}
	*/

	//close connection 
	
}

int findFile(char* fileName) {
	DIR *dp;
	struct dirent *ep;
		dp = opendir("./");
	if (dp != NULL) {
		while (ep = readdir(dp))	{
			if (strcmp(ep->d_name, fileName) == 0) {
				return 1;
			}
		}
	}
	return 0;	//file not found 
}

void sendFile(char *fileName, char *host, char* port) {
	int q_fd;						//data connection file descriptor

	FILE* requestedFile = fopen(fileName, "r");
	char fileLine[BUFFER_SIZE];
	char completeFile[FILE_SIZE];

	//int i = 0; 
	memset(fileLine, '\0', sizeof fileLine);
	memset(completeFile, '\0', sizeof completeFile);

	while (fgets(fileLine, sizeof fileLine, requestedFile)) {
		strcat(completeFile, fileLine);
		//printf("%s 	%d\n", fileLine, i++);
		memset(fileLine, '\0', sizeof fileLine);
	}

	fclose(requestedFile);

	printf("%s", completeFile);

	q_fd = initiateOnDataConnection(host, port);

	/*
	int sentLength = 0;
	int charsRead = 0;
	while (sentLength <= strlen(completeFile)) {
		charsRead = send(fd, completeFile, strlen(completeFile), 0); 
		sentLength += charsRead;
	}
	*/
}


int handleRequest(int new_fd, char* clientHost) {
	//char* placeholderCommand = "l\n";
	//char* placeholderCommand = "g shortfile.txt\n";
	//char* placeholderCommand = "g longfileee.txt\n";
	//char* placeholderCommand = "bleraharea!34r\n";
	char dataPort[5];
	//int clientPort; 

	char fileName[FILE_SIZE];
	char buffer[BUFFER_SIZE];

	memset(fileName, '\0', sizeof fileName);
	memset(buffer, '\0', sizeof buffer);
	recv(new_fd, buffer, BUFFER_SIZE - 1, 0);

	//copy port number (last 5 characters) and convert to int 
	memcpy(dataPort, buffer+(strlen(buffer)-5), 5);
	//clientPort = atoi(portString);	


	if (strncmp(buffer, "l", 1) == 0) {
		printf("List directory requested on port %s.\n", clientHost);
		printf("Sending directory contents to %s:%s\n", clientHost, dataPort);
		sendFileNames(clientHost, dataPort);

		//CONNECTION Q 
	}
	else if (strncmp(buffer, "g", 1) == 0) {
		//copy file name into buffer 
		if (strlen(buffer) > 1) {
			memcpy(fileName, buffer+2, strlen(buffer)-8);	//get fileName - starts after 'g ' and before ' #####\n'
			fileName[strlen(buffer)-8] = '\0';
			//printf("%s\n", fileName);

			//send error message if not found 
			if (!findFile(fileName)) {
				printf("File not found. Sending error message to %s:%s\n", clientHost, dataPort);
				printf("FILE NOT FOUND\n");
				return 1; 
			} else {
			//send file to client 
				printf("File \"%s\" requested on port %s.\n", fileName, dataPort);
				printf("Sending \"%s\" to %s:%s\n", buffer, fileName, dataPort); 
				sendFile(fileName, clientHost, dataPort);

				//CONNECTION Q
			}
		} else {
			printf("error: missing file name\n");	
			return 1; 
		}

		/*
		int i;
		for(i = 0; i < strlen(buffer)+1; i++) {
			printf("%c	%d\n", buffer[i], buffer[i]);
		}
		*/


	}
	else {
		printf("error: invalid command\n");
		return 1;
	}

	return 0;

}


int main(int argc, char *argv[]) {
	/*
	struct sigaction SIGINT_action = { 0 };

	//initialize signal handlers 
	SIGINT_action.sa_handler = catchSIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0; 

	//assign behaviors to override default actions 
	sigaction(SIGINT, &SIGINT_action, NULL);
	*/

	
	//http://beej.us/guide/bgnet/examples/server.c
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo; 
    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size;
    char s[INET_ADDRSTRLEN];

    char clientHost[1024], clientService[20];	//getnameinfo
    //int clientPort;

    //validate command line parameters
    if (argc != 2) { fprintf(stderr,"USAGE: ./ftserver <SERVER_PORT>\n"); exit(1); } 

    //fill struct 
    servinfo = fillAddrStruct(argv[1]);
    //create and bind socket 
	sockfd = startup(servinfo);

	freeaddrinfo(servinfo); // all done with this structure

	if (servinfo == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, 1) == -1) {
		perror("listen");
		exit(1);
	}

	//start on host A
	printf("Server open on %s\n", argv[1]);

	//repeatedly wait on <PORTNUM> for client (until terminated by SIGINT)
	while(1) {  
		//wait on connection P for command from ftclient 
		sin_size = sizeof their_addr;

		//establish TCP control connection on <SERVER_PORT>, i.e., connection P
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		
		//inet_ntop(their_addr.ss_family,						//get IP address 
		//	get_in_addr((struct sockaddr *)&their_addr),
		//	s, sizeof s);

		//get name of client host and port number 
		getnameinfo((struct sockaddr *)&their_addr, sizeof their_addr, clientHost, sizeof clientHost, clientService, sizeof clientService, 0); 

		printf("Connection from %s.\n", clientHost);

		//wait on connection P for command from ftclient 
		handleRequest(new_fd, clientHost);

		//close connection P and terminate
		close(new_fd);  
	}
	
	
	return 0;
}



	