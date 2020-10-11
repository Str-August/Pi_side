#include"../Header/socketCommunicate.h"
socketCommunicate sock_addr;
void * sendSerial_SBUS (void * socket)
{
	//static unsigned int pre_time = micros();
	while(true){
			//socketCommunicate server = *((socketCommunicate *) socket);
			//server.get_throttle();	





			//usleep(pre_time + 10000 - micros());
			//pre_time = micros();
	}
}
int main()
{
	
	sock_addr.Init();
	
	pthread_t thread;
	//pthread_create(&thread, NULL, sendSerial_SBUS, (void *) &test );


	sock_addr.ConnectClient();	

	return 0;
}
