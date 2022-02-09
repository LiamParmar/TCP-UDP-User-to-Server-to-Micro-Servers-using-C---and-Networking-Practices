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
#define PORT 2246 // proxy browser port number
#define TRANSLATEPORT 3456
#define CURRENCYPORT 3457
#define VOTINGPORT 3458
#define BLOCKPORT 4103 // telnet port number
#define MAXLINE 1024

using namespace std;
int newsocket_telnet;

int main(int argc , char *argv[])
{
	int server_fd, new_socket;
	struct sockaddr_in server;
	int opt = 1;
	int addrlen = sizeof(server);
	int Flag;
	char blockBuffer[20000] = {0};
	char userCommand [2048];
	int clientChoice;
	char buffer[1024];
	char currencyBuffer[1024];
	char currencyChosen[1024];
	char votingChosen[1024];
	char cleanBuffer[MAXLINE];
	char userVote[1024];
	char votingBuffer[1024];
	char cleanVotingChosen[1024];
	char *clientMessage = "\nPlease select a service from 1-3: 1: Translator Service 2: Currency Converter 3: Voting 4: Exit\n";
	char *translateHead = "\nPlease enter one of the following English words to translate it to French: (hello, juice, dog, train, bye) or enter 4 to exit\n";
	char *currencyHead = "\nPlease enter the amount and currency you would like to convert to FROM Canadian in the format (amount-destinationCurrency) so for example (10-USD) or Press 4 to exit\n";
	char *votingHead = "\nPlease vote before being able to see results of the election. Press 1 to see the list of candidates. Press 2 to vote. Press 3 to see the results once you have voted. Press 4 to exit.\n";
	char *orderOfCandidates = "\nThe voting results are shown from left to right in the order: Trudeau, Harper, Layton\n";
	printf("Starting\n");
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
												&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( PORT );

	if (bind(server_fd, (struct sockaddr *)&server,
								sizeof(server))<0)
	{
		cout << " why" << endl;
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	if (listen(server_fd, 20) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	// telnet socket connection for block communicatiosn 
			memset(&server,0,sizeof(server));
			server.sin_family = AF_INET;
			server.sin_addr.s_addr = htonl(INADDR_ANY);
			server.sin_port = htons( BLOCKPORT );
			struct timeval timeout;
			timeout.tv_sec = 10;// timeout for telnet communication
			timeout.tv_usec = 0;
			int socket_telnet;
			
			//create socket for connection of telnet for blocked communications
			if ((socket_telnet = socket(AF_INET, SOCK_STREAM, 0)) == 0)
			{
				perror("socket failed");
				exit(EXIT_FAILURE);
			}

			if (setsockopt(socket_telnet, SOL_SOCKET, SO_RCVTIMEO,
														(char *)&timeout, sizeof(timeout)) < 0)
			{
				perror("setsockopt");
				exit(EXIT_FAILURE);
			}

			if (bind(socket_telnet, (struct sockaddr *)&server,
										sizeof(struct sockaddr_in))<0)
			{
				cout << "but why tho" << endl;
				perror("bind failed");
				exit(EXIT_FAILURE);
			}
			if (listen(socket_telnet, 2) < 0)
			{
				perror("listen");
				exit(EXIT_FAILURE);
			}
			
			else
			{
				printf("You have 10 seconds right now to connect to block port %d...\n", BLOCKPORT);
				
				if((newsocket_telnet = accept(socket_telnet, NULL, NULL )) <0)
				{
					printf("Timeout on accept system call for block socket\n");
				}
				
				else
				{
					printf("Connected now\n");
					Flag++;
					timeout.tv_sec = 1;
					timeout.tv_usec = 0;
					
					if (setsockopt(newsocket_telnet, SOL_SOCKET, SO_RCVTIMEO,(char *)&timeout, sizeof(timeout)) < 0)
					{
						printf("setsockopt failed\n");
					}
					
					//send(newsocket_telnet, clientMessage, strlen(clientMessage), 0);
					//newsocket_block = accept(socket_block, (struct sockaddr *)&server,(socklen_t*)&addrlen);				
					
				}
				
			}
			
			while(1) //main while loop to continue processes
			{
				clientChoice = 0;// user input
				if(Flag)// if they connected set a timeout of 60 seconds
				{
					timeout.tv_sec = 60;
					timeout.tv_usec = 0;
					
				if (setsockopt(newsocket_telnet, SOL_SOCKET, SO_RCVTIMEO,(char *)&timeout, sizeof(timeout)) < 0)
				{
					printf("setsockopt failed\n");
				}
				else
				{
					send(newsocket_telnet, clientMessage, strlen(clientMessage), 0);// send them the client message outlining options to microservers
					if(recv(newsocket_telnet , blockBuffer, 2048, 0) > 0)// recieve block communications
					{
							sscanf(blockBuffer, "%s", userCommand);// scan blockbuffer into usercommand
							if((strcmp(userCommand, "1")) == 0)// set client choice to 1 if they entered 1
							{
								clientChoice = 1;
							}
							
							else if(strcmp(userCommand, "2") == 0)// set client choice to 2 if they entered 2
							{
								clientChoice = 2;
							}
							
							else if(strcmp(userCommand, "3") == 0)// set client choice to 3 if they entered 3
							{
								clientChoice = 3;
							}
							
							else if(strcmp(userCommand, "4") == 0)// set client choice to 4 if they entered 4
							{
								clientChoice = 4;
								return 0;
							}
							
							else
							{
								cout << "Please enter a valid command" << endl;
								clientChoice = 0;
							}
					}
					
					else
					{
						cout << "Timedout" << endl;
						return 0;
					}
				}
			}
				
				// CREATION OF TRANSLATE SOCKET
				int translatesockfd;
				struct sockaddr_in translateaddr;
			  
				// Creating socket file descriptor
				if ( (translatesockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
					perror("socket creation failed");
					exit(EXIT_FAILURE);
				}
			  
				memset(&translateaddr, 0, sizeof(translateaddr));
				  
				// Filling server information
				translateaddr.sin_family = AF_INET;
				
				translateaddr.sin_port = htons(TRANSLATEPORT);
				
				translateaddr.sin_addr.s_addr = inet_addr("172.19.1.31");// this is the ip of the udp client?
				  
				int translateReceive, translateLen;
				
				translateLen = sizeof(translateaddr);
				
				if(clientChoice == 1)// implementation of communicating with translate micro server
				{
					char translateBuffer[1024] = {0};
					char translateChosen[1024] = {0};
					send(newsocket_telnet, translateHead, strlen(translateHead), 0);// send client translate message of options of words to translate
					timeout.tv_sec = 5;
					timeout.tv_usec = 0;
					if (setsockopt(newsocket_telnet, SOL_SOCKET, SO_RCVTIMEO,(char *)&timeout, sizeof(timeout)) < 0)
					{
						printf("setsockopt failed\n");
					}
					
					else
					{
						recv(newsocket_telnet, translateChosen, 2048, 0);// take in word they chose
					}
					
					if(strcmp(translateChosen, "4") == 0)
					{
						
					}
					
					else
					{
						//sendto microserver word that was chosen
						sendto(translatesockfd, (const char *)translateChosen, strlen(translateChosen), MSG_CONFIRM, (const struct sockaddr *) &translateaddr, sizeof(translateaddr));
						printf("Message sent.\n");
						
						//recv the translated word from microserver
						translateReceive = recvfrom(translatesockfd, (char *)translateBuffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &translateaddr,(socklen_t *)&translateLen);
						translateBuffer[translateReceive] = '\0';
						printf("Server : %s\n", translateBuffer);
						send(newsocket_telnet, translateBuffer, translateReceive, 0);
					}
				}
				
				//CREATION OF CURRENCY SOCKET
				int currencysockfd;
				struct sockaddr_in currencyaddr;
			  
				// Creating socket file descriptor
				if ( (currencysockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
					perror("socket creation failed");
					exit(EXIT_FAILURE);
				}
			  
				memset(&currencyaddr, 0, sizeof(currencyaddr));
				  
				// Filling server information
				currencyaddr.sin_family = AF_INET;
				
				currencyaddr.sin_port = htons(CURRENCYPORT);
				
				currencyaddr.sin_addr.s_addr = inet_addr("172.19.1.21");;
				  
				int currencyReceive, currencyLen;
				
				currencyLen = sizeof(currencyaddr);
				
				if(clientChoice == 2)
				{
					// send the currency header message to client
					send(newsocket_telnet, currencyHead, strlen(currencyHead), 0);
					timeout.tv_sec = 60;
					timeout.tv_usec = 0;
					if (setsockopt(newsocket_telnet, SOL_SOCKET, SO_RCVTIMEO,(char *)&timeout, sizeof(timeout)) < 0)
					{
						printf("setsockopt failed\n");
					}
					
					else
					{
						recv(newsocket_telnet, currencyChosen, 2048, 0);// receive the currency exchange and amount requested by client
					}
					
					if(strcmp(currencyChosen, "4") == 0)
					{
						
					}
					
					else
					{
						//send the client's needs to the currency micro server
						sendto(currencysockfd, currencyChosen, strlen(currencyChosen),MSG_CONFIRM, (const struct sockaddr *) &currencyaddr, sizeof(currencyaddr));
						printf("Message sent.\n");
						
						//receive the currency exchange from the microserver
						currencyReceive = recvfrom(currencysockfd, (char *)currencyBuffer, MAXLINE, 
								MSG_WAITALL, (struct sockaddr *) &currencyaddr,
								(socklen_t *)&currencyLen);
						currencyBuffer[currencyReceive] = '\0';
						printf("Server : %s\n", currencyBuffer);
						send(newsocket_telnet, currencyBuffer, currencyReceive, 0);//send microserver response to the client
					}
					
				}
				
				// CREATION OF THE VOTING SOCKET
				int votingockfd;
				struct sockaddr_in votingaddr;
			  
				// Creating socket file descriptor
				if ( (votingockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
					perror("socket creation failed");
					exit(EXIT_FAILURE);
				}
			  
				memset(&votingaddr, 0, sizeof(votingaddr));
				  
				// Filling server information
				votingaddr.sin_family = AF_INET;
				
				votingaddr.sin_port = htons(VOTINGPORT);
				
				votingaddr.sin_addr.s_addr = inet_addr("172.19.1.30");;
				  
				int votingReceive, votingLen;
				
				votingLen = sizeof(votingaddr);
				
				if(clientChoice == 3)
				{
					send(newsocket_telnet, votingHead, strlen(votingHead), 0);// send voting header message to the client
					timeout.tv_sec = 60;
					timeout.tv_usec = 0;
					if (setsockopt(newsocket_telnet, SOL_SOCKET, SO_RCVTIMEO,(char *)&timeout, sizeof(timeout)) < 0)
					{
						printf("setsockopt failed\n");
					}
					
					else
					{
						recv(newsocket_telnet, votingChosen, 2048, 0);//receive clients option selected for the voting service
					}
					
					if(strcmp(votingChosen, "4") == 0)
					{
						
					}
					
					else
					{
						sendto(votingockfd, votingChosen, strlen(votingChosen),
						MSG_CONFIRM, (const struct sockaddr *) &votingaddr, 
						sizeof(votingaddr));// send the clients option to the microserver for appropriate service
						printf("Message sent.\n");
					
						votingReceive = recvfrom(votingockfd, (char *)votingBuffer, MAXLINE, 
							MSG_WAITALL, (struct sockaddr *) &votingaddr,
							(socklen_t *)&votingLen);// receive microserver response
						buffer[votingReceive] = '\0';
						printf("Server : %s\n", votingBuffer);
						send(newsocket_telnet, votingBuffer, votingReceive, 0);// send the microservers response to the client 
						sscanf(votingBuffer, "%s", cleanBuffer);// get rid of spaces
						sscanf(votingChosen, "%s", cleanVotingChosen);// get rid of spaces
						if(strcmp(cleanBuffer, "Please") == 0)// if its the message about voting
						{
							recv(newsocket_telnet, userVote, 2048, 0);// recieve clients vote
							sendto(votingockfd, userVote, strlen(userVote),// send vote to microserver
							MSG_CONFIRM, (const struct sockaddr *) &votingaddr, 
							sizeof(votingaddr));
						}
						
						if(strcmp(cleanVotingChosen, "3") == 0)
						{
							send(newsocket_telnet, orderOfCandidates, strlen(orderOfCandidates), 0);// send the formatting of the results
						}
						
					}
					
				}
				
			}
	}

