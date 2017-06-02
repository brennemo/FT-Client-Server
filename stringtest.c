#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1000

int main() {
	char dataPort[6];
	//char delim1[] = " ";
	//char delim2[] = "0123456789";
	int idx;

	char fileName[BUFFER_SIZE];
	char buffer[BUFFER_SIZE] = "g blorpts1.txt 1234";

	/*
	memset(dataPort, '\0', sizeof dataPort);
	memcpy(dataPort, buffer+(strlen(buffer)-5), 5);

	memcpy(fileName, buffer+2, strlen(buffer)-8);	//get fileName - starts after 'g ' and before port ' #####\n'
	fileName[strlen(buffer)-8] = '\0';


	printf("buffer: %s\n", buffer);
	printf("file name: %s\n", fileName);
	printf("port: %s\n", dataPort);
	*/

	//idx = strcspn(buffer, delim1);	//get position of 1st character in port 
	//printf("idx: %d, char: %c\n", idx, buffer[idx]);
	int i = 0;
	char * p;
	printf ("Splitting string \"%s\" into tokens:\n", buffer);
	p = strtok (buffer, " ");
	//while (p != NULL) {

	//command 
	printf ("%d: %s\n",i, p);
	p = strtok (NULL, " ");
	i++;

	//file name
	memcpy(fileName, p, strlen(p));
	printf ("%d: %s\n",i, p);
	p = strtok (NULL, " ");
	i++;

	//port 
	memcpy(dataPort, p, strlen(p));
	printf ("%d: %s\n",i, p);
	p = strtok (NULL, " ");
	i++;
	//}

	printf ("%d: %s\n",i, p);
	p = strtok (NULL, " ");
	i++;


	printf("buffer: %s\n", buffer);
	printf("file name: %s\n", fileName);
	printf("port: %s\n", dataPort);

	return 0;
}