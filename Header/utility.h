//#include<stdio.h>
//#include<string>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <exception>
#include <cstring>

#include <wiringPi.h>

//#include <bcm2835.h>
#include <stdio.h>

// PWM output on RPi Plug P1 pin 18 (which is GPIO pin 18)
// in alt fun 5.
// Note that this is the _only_ PWM pin available on the RPi IO headers
#define PIN RPI_GPIO_P1_18
// and it is controlled by PWM channel 0
#define PWM_CHANNEL 0
// This controls the max range of the PWM signal
#define RANGE 1024
#define PWM_pin 1
#define IN1 4
#define IN2 5
using namespace std;

class myexception : public exception
{
	virtual const char *what() const throw()
	{
		return "Can not setup for raspberry!";
	}
} myex;
constexpr unsigned int str2int(const char *str, int h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}
char *str2Char(std::string str)
{
	int size = str.length();
	char *cStr = new char[size + 1];
	strcpy(cStr, str.c_str());
	return cStr;
}

bool setup_motor()
{
	if (wiringPiSetup() == -1)
		return false;
	pinMode(PWM_pin, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(1920);
	pwmSetRange(200);
	//pwmSetClock (divisor)
	//pwmSetMode (mode) mode can be PWM_MODE_BAL  or PWM_MODE_MS
	//pwmSetRange (range)
	return true;
}
int active_motor()
{
	// max range is 1024 default range
	for (int i = 0; i < 200; i++)
	{
		pwmWrite(PWM_pin, i);
		delay(1);
	}
	return 0;
}
int stop_motor()
{
	for (int i = 199; i > 0; i--)
	{
		pwmWrite(PWM_pin, i);
		delay(1);
	}
	return 0;
}
// bool setUp_motor()
// {
// 	if (wiringPiSetup() == -1)
// 	{
// 		return false;
// 	}
// 	return 1;
// }
// int pwm_setup()
// {
// 	try
// 	{
// 		if (wiringPiSetup()==-1)
// 			throw myex;

// 		// Set the output pin to Alt Fun 5, to allow PWM channel 0 to be output there
// 		bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_ALT5);

// 		// Clock divider is set to 16.
// 		// With a divider of 16 and a RANGE of 1024, in MARKSPACE mode,
// 		// the pulse repetition frequency will be
// 		// 1.2MHz/1024 = 1171.875Hz, suitable for driving a DC motor with PWM
// 		bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
// 		bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 1);
// 		bcm2835_pwm_set_range(PWM_CHANNEL, RANGE);
// 		throw myex; // ifnot
// 	}
// 	catch (exception &e)
// 	{
// 		cout << e.what() << '\n';
// 		return -1;
// 	}
// 	return 1;
// }

// void pwm_out(int direction = 1, int data = 1)
// {
// 	for (int i = 0; i < 1024; i++)
// 	{
// 		// if (data == 1)
// 		//     direction = 1;   // Switch to increasing
// 		// else if (data == RANGE-1)
// 		//     direction = -1;  // Switch to decreasing
// 		// data += direction;
// 		bcm2835_pwm_set_data(PWM_CHANNEL, i);
// 		bcm2835_delay(1);
// 	}
// }

// void pwm_close()
// {
// 	bcm2835_close();
// }

// void cw_motor(int speed)
// {
// 	digitalWrite(IN1,HIGH);
// 	analogWrite(IN2,255-speed);
// }

// void ccw_motor(int speed)
// {
// 	digitalWrite(IN1, LOW);
// 	analogWrite(IN2, speed);
// }

// void dowm_motor()
// {
// 	digitalWrite(IN1, LOW);
// 	digitalWrite(IN2, LOW);
// }
// int serial()
// {
// 	int fd;

// 	while(1) {
// 		if((fd = serialOpen ("/dev/ttyAMA0", 10000)) < 0 ){
// 			return -1;
// 		}
// 		serialPuts(fd, "????");
// 		// serialFlush(fd);
// 		// printf("%s\n", "hello");
// 		// fflush(stdout);
// 		delay(1000);
// 	}
// 	return 0;
// }
