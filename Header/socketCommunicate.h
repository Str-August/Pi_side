#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <poll.h>
#include <arpa/inet.h>
#include<iostream>
using namespace std;

#define POLL_SIZE 35	// fd max (number client ) can connect server
#define LOCAL_HOST_SOCK 5533  // local host for socket

class socketCommunicate {
private:
	int listenfd, connfd, sockfd, maxi,i;
	int nready;
	ssize_t n;
	struct pollfd client_fd[POLL_SIZE];
	socklen_t clilen;
	struct sockaddr_in server_address,client_address;


public:
	void Init(); // create a socket, poll structure
    void EventServer(); // manager all event in server socket include accept connect from a client
    
	


    void ConnectClient(); //just test but can use it's structure fod do the socket connect later
	
	
	
};

