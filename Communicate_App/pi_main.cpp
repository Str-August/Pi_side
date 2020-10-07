#include"../Header/socketCommunicate.h"
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
	socketCommunicate test;
	test.Init();
	
	pthread_t thread;
	pthread_create(&thread, NULL, sendSerial_SBUS, (void *) &test );


	test.ConnectClient();	

	return 0;
}
