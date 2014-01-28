/**
 --------------------------------------------------------------------------------------------
 This is a sample file, which shows how to read data from a serial port using both the
 blocking read() function and the asynchronous callback() function, as well as how to write
 data to the serial port using the write() function.
 This program simultaneously reads from and writes to the serial port.
 --------------------------------------------------------------------------------------------
 **/
#include <stdio.h>
#include <tchar.h>
#include "sserial.h"
#include <string>
#include <iostream>
#include <windows.h>
#include "multithread.h"
#include "sserialException.h"
using namespace std;
void print(string& data,void* unused)
{
	cout<<data;
}

int main()
{
	//first, open the serial port
	//Adjust COM4 to whatever port your device is on
	serial port4("COM4",CBR_4800);

	//write some data from the console to the serial port
	string input;
	cin>>input;
	port4.write(input);

	//Read some data and print it
	//program will pause until a whitespace character is read (just like cin)
	cout<<port4.read();

	//Now, start reading data asynchronously (in a different thread).
	//This data will be printed with the print() function.
	port4.callback(print);

	//now, start writing data from the console to the port forever
	while(true)
	{
		cin>>input;
		port4.write(input);
	}
}

