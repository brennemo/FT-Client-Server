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

void handleRequest(int new_fd) {
	char* placeholderHost = "flip2";
	char* placeholderPort = "30020";
	char* placeholderFile = "shortfile.txt";

	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	recv(new_fd, buffer, BUFFER_SIZE - 1, 0);

	printf("List directory requested on port %s.\n", placeholderHost);

	//if command from ftclient == "-l"
	printf("Sending directory contents to %s:%s\n", placeholderHost, placeholderPort);
	//getFileNames();

}

void getFileNames() {
	char* fileNames[1000];	
	int i;
	DIR *dp;
	struct dirent *ep;

	dp = opendir("./");
	if (dp != NULL)
	{
		i = 0;
		while (ep = readdir(dp)) {
			fileNames[i] = ep->d_name;
			printf("%s ", fileNames[i]);
			i++;
		}
		(void)closedir(dp);
	}
	else {
		perror("Couldn't read the directory");
		exit(1);
	}
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
    struct addrinfo hints, *servinfo; //*p;
    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size;
    char s[INET_ADDRSTRLEN];

    char clientHost[1024], clientService[20];	//getnameinfo
    int clientPort;

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
		clientPort = ntohs(their_addr.sin_port); 
		printf("%d\n", clientPort);

		printf("Connection from %s.\n", clientHost);

		//wait on connection P for command from ftclient 

		//void handleRequest(new_fd);
		/*
		//receive command on connection P 
		char* placeholderHost = "flip2";
		char* placeholderPort = "30020";
		char* placeholderFile = "shortfile.txt";

		printf("List directory requested on port %s.\n", placeholderHost);
		

			//if ftclient sent invalid command

				//error message to ftclient on connection P

			//else 
				//initiate TCP data connection with ftclient on <DATA_PORT>, i.e., connection Q 
					//if command from ftclient == "-l"
					printf("Sending directory contents to %s:%s\n", placeholderHost, placeholderPort);
					//getFileNames();
					
					//if command from ftclient == "-g <FiLENAME>"
						//validate FILENAME 
						//send error message on connection P 
						printf("File not found. Sending error message to %s:%s\n", placeholderHost, placeholderPort);
						printf("FILE NOT FOUND\n");

						//send contents of FILENAME on connection Q
						printf("File \"%s\" requested on port %s.\n", placeholderFile, placeholderPort);
						printf("Sending \"%s\" to %s:%s\n", placeholderFile, placeholderHost, placeholderPort); 

						//loop to send whole file 
			

				//close connection Q
			*/	

		//close connection P and terminate
		close(new_fd);  
	}
	
	return 0;
}



	