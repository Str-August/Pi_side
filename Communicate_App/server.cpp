#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../Header/utility.h"
#include <iostream>
using namespace std;
int clientCount = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct client{

	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	int len;

};
int roll,pitch,yaw,throttle;
struct client Client[3];
pthread_t thread[3];
void analyReceivingData(int sockfd)
{
    int lenMess = 0;
    string message;
    char *p,byte_r;
    while (true)
    {
        read(sockfd, &byte_r, sizeof(byte_r));
        if (byte_r < '0' || byte_r > '9')
            break;
        lenMess = lenMess * 10 + (byte_r - '0');
    }
    for (int i = 0; i < lenMess; ++i)
    {
        /* code */
        read(sockfd, &byte_r, sizeof(byte_r));
        message.push_back(byte_r);
    }
    cout << message << " : ";

    switch (str2int(str2Char(message)))
    {
    case str2int("pitch"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        if (lenMess == 0)
            break;
        pitch = 0;
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        pitch = atoi(str2Char(message));
        cout << pitch << endl;

        break;
    case str2int("roll"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        if (lenMess == 0)
            break;
        roll = 0;
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        roll = atoi(str2Char(message));
        cout << pitch << endl;

        break;
    case str2int("power"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        cout << message << endl;
        break;
    case str2int("throttle"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        throttle = 0;
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        throttle = atoi(str2Char(message));
        cout << throttle << endl;

        //cout<<yaw<<endl;
        break;
    case str2int("yaw"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        yaw = 0;
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        yaw = atoi(str2Char(message));
        cout << yaw << endl;
        break;

    case str2int("motor"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        cout << message << endl;
        //handleMotorCut(message);
        break;
    }
}

void * doNetworking(void * ClientDetail){

	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;
	bool check_ed= false;
	printf("Client %d connected.\n",index + 1);
	static unsigned int pre_time = micros() , send_time, rev_time;
	char data;
	while(1){
		
		if(check_ed)
		{
			
		}
		data = 'a';
		if(send(clientSocket,&data, sizeof(data),0)<0)
		{
			cout<< "Can't send!"<<endl;
		}
		send_time = micros();
		char data;
		if(recv(clientSocket,&data,sizeof(data),0)<0)
		{
			cout<< "Can't receive!"<<endl;
		}
		
		if(data == '!')
		{
			analyReceivingData(clientSocket);
		}else
		{
			if (data = 'b')
			{
				rev_time = micros();
			}
			
		}
		


	}

	return NULL;

}

int main(){

	int temp = 1;

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5533);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &temp, sizeof(int));
	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1) return 0;

	if(listen(serverSocket,1024) == -1) return 0;

	printf("Server started listenting on port  ...........\n");

	while(1){

		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, (unsigned int*)(&Client[clientCount].len));
		Client[clientCount].index = clientCount;

		pthread_create(&thread[clientCount], NULL, doNetworking, (void *) &Client[clientCount]);

		clientCount ++;
 
	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

}