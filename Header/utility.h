//#include<stdio.h>
//#include<string>
#include <termios.h>
#include <unistd.h>
#include<iostream>
#include <cstring>
using namespace std;
constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}
char* str2Char(std::string str)
{	
    int size = str.length();
    char *cStr = new char[size+1];
    strcpy(cStr, str.c_str());
    return cStr;
}
