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

/*
** Function: 	
**
** Description: 
**				
** Parameters: 	
** Returns: 	
*/
void catchSIGINT(int signo) {
	//foreground signal terminates self
		puts("\nForeground signal terminating.\n");				
		fflush(stdout);
		
		exit(0);
	}


void *get_in_addr(struct sockaddr *sa) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
}


/*
** Function: 	fillAddrStruct
**
** Description: 
**				
** Parameters: 	port: port number specified in command line 
** Returns: 	pointer to addrinfo to use to create socket 
*/

struct addrinfo* fillAddrStruct(char *port) { 
    struct addrinfo hints, *servinfo; 
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


/*
** Function: 	startup
**
** Description: creates and binds socket
**				
** Parameters: 	servinfo: pointer returned by fillAddrStruct 
** Returns: 	sockfd: socket file descriptor 
*/
int startup(struct addrinfo *servinfo) {
	int sockfd;  

	if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
		servinfo->ai_protocol)) == -1) {
		fprintf(stderr, "server: socket");
	}

	//bind socket 
	if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		close(sockfd);
		fprintf(stderr, "server: bind");
		exit(1);
	}

    return sockfd; 
}


/*
** Function:	initiateOnDataConnection 	
**
** Description: connects to data connection running on client 
**				
** Parameters: 	host: client's host 
**				port: port number of data connection, specified by client 
** Returns: 	socketfd: socket file descriptor 
*/
int initiateOnDataConnection(char *host, char *port) {
	struct addrinfo hints, *res;
	char ipstr[INET_ADDRSTRLEN];
	
	int socketfd; 
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; 	
	hints.ai_socktype = SOCK_STREAM;		//TCP
	hints.ai_flags = AI_PASSIVE; 
	
	//fill addrinfo struct 
	printf("connection q host: %s, port: %s\n", host, port);

	if (getaddrinfo(host, port, &hints, &res) != 0) {
		fprintf(stderr,"error: getaddrinfo\n"); exit(1); 	
	}
	
	//create socket 
	socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socketfd == -1) { fprintf(stderr,"error: socket\n"); exit(1); }
	
	//establish connection 
	//status = connect(socketfd, res->ai_addr, res->ai_addrlen);

	if (connect(socketfd, res->ai_addr, res->ai_addrlen) == -1) {
		fprintf(stderr,"error: data connection\n"); 
		close(socketfd);
		exit(1); 
	}

		
	freeaddrinfo(res);						//free linked list
	
	return socketfd;
}


/*
** Function: 	listDirectory
**
** Description: reads file names from current directory, stores names in 
**				a string delimited by newlines, and sends string to client 
**				
** Parameters: 	host: client's host 
**				port: port number of data connection, specified by client 
** Returns: 	0 on success, -1 on failure 
*/
int listDirectory(char* host, char* port) {
	int q_fd;						//data connection file descriptor 
	char fileNames[BUFFER_SIZE];
	DIR *dp;
	struct dirent *ep;
	int n;

	memset(fileNames, '\0', sizeof fileNames);

	dp = opendir("./");
	if (dp != NULL)
	{
		//build string containing all filenames, separated by newlines 
		while (ep = readdir(dp)) {
			strcat(fileNames, ep->d_name);
			strcat(fileNames, "\n");
		}
		(void)closedir(dp);
	}
	else {
		fprintf(stderr, "error: opendir\n");
		exit(1);
	}
	//printf("%s", fileNames);

	q_fd = initiateOnDataConnection(host, port);

	//test sending a short string without loop
	printf("Sending directory contents to %s:%s\n", host, port);
	n = send(q_fd, fileNames, strlen(fileNames), 0); 

	close(q_fd);

	return n==-1?-1:0;
}


/*
** Function: 	findFile
**
** Description: searches for file in the current directory
**				and returns the result 
**				
** Parameters: 	fileName: name of file to search for 
** Returns: 	1 if found, 0 if not found 
*/
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


/*
** Function: 	getFileSize
**
** Description: gets the size of the file to be sent to the client 
**				
** Parameters: 	fileName: name of file to open
** Returns: 	size of file 
*/
unsigned long getFileSize(char* fileName) {
	unsigned long fileSize; 
	FILE* requestedFile = fopen(fileName, "r");

	fseek(requestedFile, 0, SEEK_END);
	fileSize = ftell(requestedFile);
	fseek(requestedFile, 0, SEEK_SET);
	fclose(requestedFile);

	return fileSize; 
}


/*
** Function: 	sendFile
**
** Description: connects to client on data connection, then repeatedly 
**				reads and sends 1000-byte chunks of file to client 
**				
** Parameters: 	fileName: name of file to send 
** Parameters: 	host: client's host 
**				port: port number of data connection, specified by client 
** Returns: 	0 on success, -1 on failure 
*/
int sendFile(char* fileName, char* host, char* port) {
	int q_fd;						//data connection file descriptor

	FILE* requestedFile = fopen(fileName, "r");
	char fileLine[BUFFER_SIZE];

	unsigned long fileSize; 
	int count = 0;
	int len, bytesleft, n;
	int total = 0;        	// how many bytes we've sent

    fileSize = getFileSize(fileName);
    printf("file size: %lu\n", fileSize);

	q_fd = initiateOnDataConnection(host, port);

	printf("Sending \"%s\" to %s:%s\n", fileName, host, port); 

	len = fileSize;
	bytesleft = len;
    printf("complete file length: %d bytes\n", len);

    memset(fileLine, '\0', sizeof fileLine);

    //read and send 1000-byte chunks until bytes sent reaches the size of the file 
    while(total < len) {
		fgets(fileLine, sizeof fileLine, requestedFile);

        n = send(q_fd, fileLine, BUFFER_SIZE, 0);

        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
        count++;

        memset(fileLine, '\0', sizeof fileLine);
    }
   	send(q_fd, "@@", 3, 0);				//"@@" lets client know this is the end of the transfer 
    printf("%d bytes sent. Total = %d in %d send()s. %d bytes left to send.\n", n, total, ++count, bytesleft);
   
    printf("@@\n");

    fclose(requestedFile);
	close(q_fd);

	return n==-1?-1:0;
}


/*
** Function: 	handleRequest	
**
** Description: gets and parses client's, checks options, and completes 
**				requested transfer 
**				
** Parameters: 	
** Returns: 	
*/
int handleRequest(int new_fd, char* clientHost) {
	char dataPort[6];
	char fileName[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];
	char errorMessage[15] = "FILE NOT FOUND";
	char* p;									//ptr for stringtok 

	memset(fileName, '\0', sizeof fileName);
	memset(buffer, '\0', sizeof buffer);

	//get command from client 
	recv(new_fd, buffer, BUFFER_SIZE - 1, 0);

	//send directory contents 
	if (strncmp(buffer, "l", 1) == 0) {
		p = strtok (buffer, " ");			//command

		p = strtok (NULL, " ");				//port
		memcpy(dataPort, p, strlen(p));

		printf("List directory requested on port %s.\n", clientHost);
		if (listDirectory(clientHost, dataPort) == -1) { fprintf(stderr, "error: list directory\n"); return 1; };
	}
	//send file 
	else if (strncmp(buffer, "g", 1) == 0) {
		p = strtok (buffer, " ");			//command

		p = strtok (NULL, " ");				//file name
		memcpy(fileName, p, strlen(p));	

		p = strtok (NULL, " ");				//port 
		memcpy(dataPort, p, strlen(p));

		//send error message if not found 
		if (!findFile(fileName)) {
			fprintf(stderr, "File not found. Sending error message to %s:%s\n", clientHost, dataPort);
			printf("%s\n", errorMessage);
			//send(new_fd, errorMessage, strlen(errorMessage), 0);
			return 1; 
		} else {
		//send file to client 
			printf("File \"%s\" requested on port %s.\n", fileName, dataPort);
			if (sendFile(fileName, clientHost, dataPort) == -1 ) { fprintf(stderr, "error: send file\n"); return 1; }
		}
		/*} else {
			fprintf(stderr, "error: missing file name\n");	
			return 1; 
		}*/
	} else {
		fprintf(stderr, "error: invalid command\n");
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


	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo; 
    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size;
    char s[INET_ADDRSTRLEN];

    char clientHost[1024], clientService[20];	//getnameinfo

    //validate command line parameters
    if (argc != 2) { fprintf(stderr,"USAGE: ./ftserver <SERVER_PORT>\n"); exit(1); } 

    //check that ports are numbers, not reserved ports, or out of range 
	if (atoi(argv[1]) == 0 || atoi(argv[1]) < 1024 || atoi(argv[1]) > 65535) { 
		fprintf(stderr,"USAGE: ./ftserver <SERVER_PORT>\n"); exit(1); 
	}
   
    servinfo = fillAddrStruct(argv[1]);		//get addrinfo with port number 
	sockfd = startup(servinfo);				//create and bind socket 

	freeaddrinfo(servinfo); 

	if (servinfo == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, 1) == -1) {
		fprintf(stderr, "error: listen");
		exit(1);
	}

	//start on host A
	printf("Server open on %s\n", argv[1]);

	//repeatedly wait on <PORTNUM> for client (until terminated by SIGINT)
	while(1) {  
		sin_size = sizeof their_addr;

		//establish TCP control connection on <SERVER_PORT>, i.e., connection P
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			fprintf(stderr, "error: accept");
			continue;
		}

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



	