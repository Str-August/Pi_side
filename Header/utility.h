//#include<stdio.h>
//#include<string>
#include <termios.h>
#include <unistd.h>
#include<iostream>
#include <cstring>

#include<wiringPi.h>

#define IN1	4
#define IN2	5
using namespace std;

constexpr unsigned int str2int(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}
char* str2Char(std::string str)
{
	int size = str.length();
	char *cStr = new char[size + 1];
	strcpy(cStr, str.c_str());
	return cStr;
}

bool setUp_motor()
{
	if (wiringPiSetup() == -1)
	{
		return false;
	}
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	return 1;
}

void cw_motor(int speed)
{
	digitalWrite(IN1,HIGH);
	analogWrite(IN2,255-speed);
}

void ccw_motor(int speed)
{
	digitalWrite(IN1, LOW);
	analogWrite(IN2, speed);
}

void dowm_motor()
{
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
}



