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

//#define BACKLOG 10	 // how many pending connections queue will hold

void catchSIGINT(int signo) {
	//foreground signal terminates self
		puts("\nForeground signal terminating.\n");				
		fflush(stdout);
		
		exit(0);
	}

/*
void sigchld_handler(int s) {
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}
*/

void *get_in_addr(struct sockaddr *sa) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
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
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

	//char *port;

    //validate command line parameters
    if (argc != 2) { fprintf(stderr,"USAGE: ./ftserver <SERVER_PORT>\n"); exit(1); } 
	//port = argv[1];

    //fill struct 
    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    //create socket 
	if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
			servinfo->ai_protocol)) == -1) {
		perror("server: socket");
		//continue;
	}


	//if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	//bind socket 
	if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		close(sockfd);
		perror("server: bind");
		exit(1);
		//continue;
	}

	freeaddrinfo(servinfo); // all done with this structure

	//if (p == NULL)  {
	if (servinfo == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, 1) == -1) {
		perror("listen");
		exit(1);
	}

	/*
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	*/

	//start on host A
	printf("Server open on %s\n", argv[1]);

	//repeatedly wait on <PORTNUM> for client (until terminated by SIGINT)
	while(1) {  
		//wait on connection P for command from ftclient 
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		//establish TCP control connection on <SERVER_PORT>, i.e., connection P
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("Connection from %s.\n", s);

		//wait on connection P for command from ftclient 

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
					
					//if command from ftclient == "-g <FiLENAME>"
						//validate FILENAME 
						//send error message on connection P 
						printf("File not found. Sending error message to %s:%s\n", placeholderHost, placeholderPort);
						printf("FILE NOT FOUND\n");

						//send contents of FILENAME on connection Q
						printf("File \"%s\" requested on port %s.\n", placeholderFile, placeholderPort);
						printf("Sending \"%s\" to %s:%s\n", placeholderFile, placeholderHost, placeholderPort); 
						

				//close connection Q
		/*
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			if (send(new_fd, "Hello, world!", 13, 0) == -1)
				perror("send");
			close(new_fd);
			exit(0);
		}
		*/


		//close connection P and terminate
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}



	