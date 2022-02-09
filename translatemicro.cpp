#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <string>
#include <iostream>
#include <fstream>
#include<string.h>
#include <stdlib.h>
#include<unistd.h>
#include <netinet/in.h>
#include<netdb.h>
#include <time.h>
#include <signal.h>
#include<cstring>

using namespace std;

#define PORT 3456
#define MAXLINE 1024

char *bonjour = "bonjour";
char *chien = "chien";
char *jus = "jus";
char *former = "former";
char *aurevoir = "au revoir";
char *errorMessage = "You gave a word that is not supported. Please check the avaliable words again";

// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
	char cleanBuffer[MAXLINE];
    char *translateHead = "Please enter one of the following English words to translate it to French: hello, donut, dog, train, bye";
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len, n;

    len = sizeof(cliaddr);  //len is value/resuslt
	while(1)
	{
		n = recvfrom(sockfd, (char *)buffer, MAXLINE,
					MSG_WAITALL, ( struct sockaddr *) &cliaddr,
					(socklen_t*)&len);
		buffer[n] = '\0';
		sscanf(buffer, "%s", cleanBuffer);// remove spaces
		printf("Indirection Server : %s\n", cleanBuffer);
		
		if(strcmp(cleanBuffer, "hello") == 0 || strcmp(cleanBuffer, "Hello") == 0)// translate hello
		{
			sendto(sockfd, (const char *)bonjour, strlen(bonjour),
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
				
			printf("Message sent from translate.\n");
		}
		
		else if(strcmp(cleanBuffer, "juice") == 0 || strcmp(cleanBuffer, "Juice") == 0) // translate juice 
		{
			sendto(sockfd, (const char *)jus, strlen(jus),
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
				
			printf("Message sent from translate.\n");
		}
		
		else if(strcmp(cleanBuffer, "dog") == 0 || strcmp(cleanBuffer, "Dog") == 0) // translate dog
		{
			sendto(sockfd, (const char *)chien, strlen(chien),
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
				
			printf("Message sent from translate.\n");
		}
		
		else if(strcmp(cleanBuffer, "train") == 0 || strcmp(cleanBuffer, "Train") == 0) // translate train
		{
			sendto(sockfd, (const char *)former, strlen(former),
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
				
			printf("Message sent from translate.\n");
		}
		
		else if(strcmp(cleanBuffer, "bye") == 0 || strcmp(cleanBuffer, "Bye") == 0)// translate bye
		{
			sendto(sockfd, (const char *)aurevoir, strlen(aurevoir),
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
				
			printf("Message sent from translate.\n");
		}
		
		else
		{
			sendto(sockfd, (const char *)errorMessage, strlen(errorMessage),
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
				
			printf("Message sent from translate.\n");
		}
	}

    return 0;
}