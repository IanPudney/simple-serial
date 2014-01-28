#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
using namespace std;

class serial
{
    private:
        //serial comm handler
        HANDLE hserial;
        //Connection status
        bool connected;
        //Get various information about the connection
        COMSTAT status;
        //Keep track of last error
        DWORD errors;
		int ReadData(char *buffer, unsigned int nbChar);
        //Writes data from a buffer through the serial connection
        //return true on success.
        bool WriteData(char *buffer, unsigned int nbChar);
        //Check if we are actually connected
        bool callbackEnabled;
        void* callbackParam;
        void (*callbackFunc)(string&,void*);
        static void callbackRun(void* SPVoid);
    public:
        //Initialize serial communication
        //Typically, only the portName and possibly baudRate need to be configured
        //other parameters rarely change
        serial(char *portName,int baudRate=CBR_9600,unsigned char stopBits=ONESTOPBIT,unsigned char parity=NOPARITY,int byteSize=8);
        //Close the connection
        //NOTA: for some reason you can't connect again before exiting
        //the program and running it again
        ~serial();
        //Read data in a buffer, if nbChar is greater than the
        //maximum number of bytes available, it will return only the
        //bytes available. The function return -1 when nothing could
        //be read, the number of bytes actually read.
        bool IsConnected();
        //returns true if connected, false otherwise
        string read();
        //Blocking operation
        //Wait until the next word is read, then return it
        //A word is defined as a set of characters which are not whitespace
        void write(string &data);
        //Blocking operation
        //Outputs the desired data to the serial port
        void callback(void (function_name)(string&,void*), void* param=NULL);
        //Registers a function to run every time a word is received
        //the function must take a string& as its first parameter, and a pointer to something as its second
        //the contents of the word will be passed to the function in the string& parameter.
        //The function's pointer parameter can be used in place of the function's return value;
        //it will always receive the pointer you pass with the param parameter to THIS function.
        //this is an ASYNCHRONOUS function. Using read() while a callback is registered will produce undefined behavior.
        void clearCallback();
        //Cancels the callback function, allowing read() to be used properly again.
        //Note that the callback function may run one additional time after this function is called.







};

#endif // SERIALCLASS_H_INCLUDED
