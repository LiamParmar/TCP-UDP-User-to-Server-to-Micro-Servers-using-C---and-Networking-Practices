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


#define PORT 3457
#define MAXLINE 1024

// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
	char cleanBuffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
	double CAD_to_USD = 0.81;
	double CAD_to_EURO = 0.70;
	double CAD_to_GBP = 0.59;
	double CAD_to_BTC = 0.000016;
	char amount[40];
	char countryID[10];
	char *amountErrorMessage = "You gave an amount that is not a number.";
	char *countryErrorMessage = "You gave an invalid currency exchange type. Please see the supported currency conversions.";
	
	using namespace std;

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
    printf("Client : %s\n", cleanBuffer);
	
		//for loop find the - then do cleanBuffer[0, whatever]
		int position = 0;
		int arrayLength = sizeof(cleanBuffer) / sizeof(cleanBuffer[0]);
		int actualLength = 0;
		int totalLength = 0;
		for(int i = 0; i < arrayLength; i++)// seperate exchange rate country from amount to handle seperately
		{
			if(cleanBuffer[i] == '-')
			{
				position = i;
			}
			
			if(cleanBuffer[i] == '\0')
			{
				cout << "Value of i at end is: " << endl;
				totalLength = i;
				break;
			}
			
			if(position != 0 && cleanBuffer[i] != '-')
			{
				countryID[actualLength] = cleanBuffer[i];
				actualLength++;
			}
			
			if(isdigit(cleanBuffer[i]))
			{
				amount[i] = cleanBuffer[i];
			}
			
			else if(isdigit(cleanBuffer[i] == 0 && position == 0))
			{
				sendto(sockfd, (const char *)amountErrorMessage, strlen(amountErrorMessage),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
				exit(0);
			}	
		}
		
		countryID[actualLength] = '\0';
		amount[position] = '\0';
		double number = 0;
		unsigned long long int m = 1;
		for(int i = position - 1; i >= 0; i--)
		{
			number += (cleanBuffer[i] - '0') * m;
			m *= 10;
		} // convert to double
		
		double conversion;
		
		if(strcmp(countryID, "USD") == 0)
		{
			conversion = number * CAD_to_USD;
		}
		
		else if(strcmp(countryID, "EURO") == 0)
		{
			conversion = number * CAD_to_EURO;
		}
		
		else if(strcmp(countryID, "BTC") == 0)
		{
			conversion = number * CAD_to_BTC;
		}
		
		else if(strcmp(countryID, "GBP") == 0)
		{
			conversion = number * CAD_to_USD;
		}
		
		else 
		{
			sendto(sockfd, (const char *)countryErrorMessage, strlen(countryErrorMessage),
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
			len);
		}
		
		cout << "Converted number is: " << conversion << endl;
		
		char wordConversion[1048];
		sprintf(wordConversion, "%f", conversion);
		
		
		
		
		
		
		sendto(sockfd, (const char *)wordConversion, strlen(wordConversion),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);
	
	
	/*
    sendto(sockfd, (const char *)hello, strlen(hello),
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
            len);*/
    printf("Currency message sent.\n");
	}
    return 0;
}