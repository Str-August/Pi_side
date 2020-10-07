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
#include <iostream>
#include <sstream> 
#include <sys/time.h>

using namespace std;

#define POLL_SIZE 2	// fd max (number client ) can connect server
#define LOCAL_HOST_SOCK 5533  // local host for socket

class socketCommunicate {
private:
	int listenfd, connfd, sockfd, maxi,i;
	int nready;
	ssize_t n;
	struct pollfd client_fd[POLL_SIZE];
	socklen_t clilen;
	struct sockaddr_in server_address,client_address;
	unsigned char byte_r;
	int throttle,yaw,roll,pitch;
	bool power,saveVideo,pw_CutMotor,m_clockwise;


public:
	void Init(); // create a socket, poll structure
    void EventServer(); // manager all event in server socket include accept connect from a client
    void analyReceivingData();
    void handleMotorCut(string message);
    void ConnectClient(); //just test but can use it's structure fod do the socket connect later
	
	bool get_powerStatus();
	bool get_saveVideoStatus();
	bool get_pw_CutMotorStatus();
	bool get_m_clockwiseStatus();

	int get_throttle();
	int get_yaw();
	int get_roll();
	int get_pitch();


	
};

	
