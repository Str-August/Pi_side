#include"../Header/socketCommunicate.h"
socketCommunicate socket;
void * sendSerial_SBUS (void * socket)
{
	//static unsigned int pre_time = micros();
	while(true){
			socketCommunicate server = *((socketCommunicate *) socket);
			server.get_throttle();	





			//usleep(pre_time + 10000 - micros());
			//pre_time = micros();
	}
}
int main()
{
	
	socket.Init();
	
	pthread_t thread;
	//pthread_create(&thread, NULL, sendSerial_SBUS, (void *) &test );


	socket.ConnectClient();	

	return 0;
}
