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
#include <sstream>
#include <cctype>


#define PORT 3458
#define MAXLINE 1024

using namespace std;
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
	char cleanBuffer[MAXLINE];
	char voteBuffer[MAXLINE];
	char cleanVoteBuffer[MAXLINE];
	int encryptionKey = 9;
	int voteCasted = 0;
    char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;
	bool hasVoted = false;
	char *candidates [3] = {"Justin Trudeau", "Stephen Harper", "Jack Layton"};
	int liveCount [3] = {0,0,0};
	char liveCountToSend[4];
	int ID [3] = {1,2,3};
	char *showCandidates = "\nThe candiate ID is shown next to the name of each candidate that will be used to cast your vote. 1: Justin Trudeau, 2: Stephen Harper, 3: Jack Layton\n";
	char *sendVoteMessage = "Please select the corresponding candidate ID when voting ex.(voting for Justin Trudeau would be entering 1)\n";
	char *sendAlreadyVotedMessage = "\nYou already voted\n";
	char *blockResults = "\nYou cannot see the results of the election until you vote!\n";

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

    int len, n, vote;

    len = sizeof(cliaddr);  //len is value/resuslt
	
	while(1)
	{
		voteCasted = 0;
		int number = 0;
		n = recvfrom(sockfd, (char *)buffer, MAXLINE,
					MSG_WAITALL, ( struct sockaddr *) &cliaddr,
					(socklen_t*)&len);
		buffer[n] = '\0';
		sscanf(buffer, "%s", cleanBuffer);// remove spaces
		printf("Client : %s\n", cleanBuffer);
		
		if(strcmp(cleanBuffer, "1") == 0)// if they selected option 1
		{
			sendto(sockfd, (const char *)showCandidates, strlen(showCandidates),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);
		}
		
		else if(strcmp(cleanBuffer, "2") == 0)// if option 2
		{
			sendto(sockfd, (const char *)sendVoteMessage, strlen(sendVoteMessage),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);// send voting message
			vote = recvfrom(sockfd, (char *)voteBuffer, MAXLINE,
					MSG_WAITALL, ( struct sockaddr *) &cliaddr,
					(socklen_t*)&len); // receive vote
			voteBuffer[vote] = '\0';
			sscanf(voteBuffer, "%s", cleanVoteBuffer);// clean spaces
			cleanVoteBuffer[vote] = '/0';
			cout << "CleanVoteBuffer: " << endl;
			if(strcmp(cleanVoteBuffer, "1") == 0 && hasVoted == false)// if they voted for candidate 1 and they havent voted
			{
				number = 1;
				voteCasted = number * encryptionKey; // encryption key
				liveCount[0] += 1;
				hasVoted = true;
			}
			else if(strcmp(cleanVoteBuffer, "2") == 0 && hasVoted == false)// if they voted for candidate 2 and they havent voted
			{
				number = 2;
				voteCasted = number * encryptionKey;
				liveCount[1] += 1;
				hasVoted = true;
			}
			
			else if(strcmp(cleanVoteBuffer, "3") == 0 && hasVoted == false)// if they voted for candidate 3 and they havent voted
			{
				number = 3;
				voteCasted = number * encryptionKey;
				liveCount[2] += 1;
				hasVoted = true;
			}
			
			if(hasVoted == true)// if they already voted and tried to again
			{
				sendto(sockfd, (const char *)sendAlreadyVotedMessage, strlen(sendAlreadyVotedMessage),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);
			}
		}
		
		if(strcmp(cleanBuffer, "3") == 0 && hasVoted == true)// if they voted and selected to see results allow it
		{
			for(int i = 0; i < 3; i++)
			{
				liveCountToSend[i] = liveCount[i] + '0';
				cout << "LiveCount being sent: " << liveCountToSend << endl;
			}
			liveCountToSend[3] = '\0';
			sendto(sockfd, (const char *)liveCountToSend, strlen(liveCountToSend),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);
		}
		
		else if(strcmp(cleanBuffer, "3") == 0 && hasVoted == false)// if they want to see results before voting
		{
			sendto(sockfd, (const char *)blockResults, strlen(blockResults),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);
		}
		
	}

    return 0;
}