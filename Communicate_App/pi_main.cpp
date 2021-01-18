#include "../Header/socketCommunicate.h"
#include <wiringPi.h>
#include <wiringSerial.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
socketCommunicate sock_addr;
int serialOpen(const char* device)
{
	struct termios options;
	speed_t myBaud;
	int status, fd;
	myBaud = B115200;
	if ((fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
		return -1;

	fcntl(fd, F_SETFL, O_RDWR);

	// Get and modify current options:

	tcgetattr(fd, &options);

	cfmakeraw(&options);
	cfsetispeed(&options, myBaud);
	cfsetospeed(&options, myBaud);

	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag |= PARENB; //even bit
	options.c_cflag |= CSTOPB; // 2 stop bit
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;

	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 100; // Ten seconds (100 deciseconds)

	tcsetattr(fd, TCSANOW, &options);

	ioctl(fd, TIOCMGET, &status);

	status |= TIOCM_DTR;
	status |= TIOCM_RTS;

	ioctl(fd, TIOCMSET, &status);

	usleep(10000); // 10mS
	cout << "DONE";
	cout << fd;
	return fd;
}
int openSerial(const char* device) {
	int fd = serialOpen(device, 115200);
	if (fd < 0) {
		cout << "error\r\n";
		return fd;
	}
	//while(1)
	//serialPutchar(fd, 'c');
	struct termios options;
	tcgetattr(fd, &options);   // Read current options
	options.c_cflag &= ~CSIZE;  // Mask out size
	options.c_cflag |= CSTOPB;     // two bit stop
	options.c_cflag |= PARENB;  // Enable Parity - even by default
  	tcsetattr (fd,TCSANOW, &options) ;   // Set new options
	cout << "Done config\r\n";
	return fd;
}
void send_package(int fd, uint16_t* channels)
{
	static uint8_t packet[25];
	/* assemble the SBUS packet */
	// SBUS header
	packet[0] = 0x0F;
	// 16 channels of 11 bit data
	if (channels)
	{
		packet[1] = (uint8_t)((channels[0] & 0x07FF));
		packet[2] = (uint8_t)((channels[0] & 0x07FF) >> 8 | (channels[1] & 0x07FF) << 3);
		packet[3] = (uint8_t)((channels[1] & 0x07FF) >> 5 | (channels[2] & 0x07FF) << 6);
		packet[4] = (uint8_t)((channels[2] & 0x07FF) >> 2);
		packet[5] = (uint8_t)((channels[2] & 0x07FF) >> 10 | (channels[3] & 0x07FF) << 1);
		packet[6] = (uint8_t)((channels[3] & 0x07FF) >> 7 | (channels[4] & 0x07FF) << 4);
		packet[7] = (uint8_t)((channels[4] & 0x07FF) >> 4 | (channels[5] & 0x07FF) << 7);
		packet[8] = (uint8_t)((channels[5] & 0x07FF) >> 1);
		packet[9] = (uint8_t)((channels[5] & 0x07FF) >> 9 | (channels[6] & 0x07FF) << 2);
		packet[10] = (uint8_t)((channels[6] & 0x07FF) >> 6 | (channels[7] & 0x07FF) << 5);
		packet[11] = (uint8_t)((channels[7] & 0x07FF) >> 3);
		packet[12] = (uint8_t)((channels[8] & 0x07FF));
		packet[13] = (uint8_t)((channels[8] & 0x07FF) >> 8 | (channels[9] & 0x07FF) << 3);
		packet[14] = (uint8_t)((channels[9] & 0x07FF) >> 5 | (channels[10] & 0x07FF) << 6);
		packet[15] = (uint8_t)((channels[10] & 0x07FF) >> 2);
		packet[16] = (uint8_t)((channels[10] & 0x07FF) >> 10 | (channels[11] & 0x07FF) << 1);
		packet[17] = (uint8_t)((channels[11] & 0x07FF) >> 7 | (channels[12] & 0x07FF) << 4);
		packet[18] = (uint8_t)((channels[12] & 0x07FF) >> 4 | (channels[13] & 0x07FF) << 7);
		packet[19] = (uint8_t)((channels[13] & 0x07FF) >> 1);
		packet[20] = (uint8_t)((channels[13] & 0x07FF) >> 9 | (channels[14] & 0x07FF) << 2);
		packet[21] = (uint8_t)((channels[14] & 0x07FF) >> 6 | (channels[15] & 0x07FF) << 5);
		packet[22] = (uint8_t)((channels[15] & 0x07FF) >> 3);
	}
	// flags
	packet[23] = 0x00;
	// footer
	packet[24] = 0x04;

	write(fd, packet, sizeof(packet));
}
unsigned short encodeSBUS(unsigned short input) {
	return (((int)input - 880) * 8 + 4) / 5;
}
void* sendSerial_SBUS(void* socket)
{
	int fd;
	uint16_t channels[16];
	static unsigned int pre_time = micros();
	if ((fd = serialOpen("/dev/ttyAMA0")) < 0)
	{
		cout << "Error to open com port \n";
	}
	if (wiringPiSetup() == -1)
	{
		cout << "Error to setup wiringPi\n";
	}
	cout << "Opened Serial";
	pre_time = micros();
	while (true)
	{
		socketCommunicate server = *((socketCommunicate*)socket);
		channels[2] = (channels[2] * 6 / 7) + (encodeSBUS(server.get_throttle()) / 7);
		channels[3] = (channels[3] * 6 / 7) + (encodeSBUS(server.get_yaw()) / 7);
		channels[1] = (channels[1] * 6 / 7) + (encodeSBUS(server.get_pitch()) / 7 );
		channels[0] = (channels[0] * 6 / 7) + (encodeSBUS(server.get_roll()) / 7);
		if (server.get_powerStatus()) {
			channels[4] = encodeSBUS(1000);
		}
		else {
			channels[4] = encodeSBUS(2000);
		}
		channels[5] = encodeSBUS(1000);
		cout << "Before send\r\n";
		send_package(fd, channels);
		cout << "After send\r\n";
		unsigned int sleep_time = pre_time + 16000 - micros();
		if (sleep_time > 0) {
			usleep(sleep_time);
		}
		pre_time = micros();
	}
}
int main()
{

	sock_addr.Init();

	pthread_t thread;
	pthread_create(&thread, NULL, sendSerial_SBUS, (void*)&sock_addr);

	sock_addr.ConnectClient();

	return 0;
}
