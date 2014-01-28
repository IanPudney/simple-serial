#include <stdio.h>
#include <tchar.h>
#include "sserial.h"
#include <string>
#include <iostream>
#include <windows.h>
#include "multithread.h"
#include "sserialException.h"
serial::serial(char *portName,int baudRate,unsigned char stopBits,unsigned char parity,int byteSize)
{
    //We're not yet connected
    connected = false;
	callbackEnabled=false;

	string complexPortName("\\\\.\\");
	complexPortName+=portName;

    //Try to connect to the given port throuh CreateFile
    hserial = CreateFile(complexPortName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    //Check if the connection was successfull
    if(hserial==INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if(GetLastError()==ERROR_FILE_NOT_FOUND){

            //Print Error if neccessary
            throw sserialException("serial: could not bind to COM port");

        }
        else
        {
            throw sserialException("serial: unknown serial error");
        }
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbserialParams = {0};

        //Try to get the current
        if (!GetCommState(this->hserial, &dcbserialParams))
        {
            //If impossible, show an error
            throw sserialException("serial: failed to get current serial parameters!");
        }
        else
        {
            //Define serial connection parameters for the device
            dcbserialParams.BaudRate=baudRate;
            dcbserialParams.ByteSize=byteSize;
            dcbserialParams.StopBits=stopBits;
            dcbserialParams.Parity=parity;

             //Set the parameters and check for their proper application
             if(!SetCommState(hserial, &dcbserialParams))
             {
                throw sserialException("serial: Could not set serial Port parameters");
             }
             else
             {
                 //If everything went fine we're connected
                 this->connected = true;
             }
        }
    }

}

serial::~serial()
{
    //Check if we are connected before trying to disconnect
    if(connected)
    {
        //We're no longer connected
        this->connected = false;
        //Close the serial handler
        CloseHandle(this->hserial);
    }
}

int serial::ReadData(char *buffer, unsigned int nbChar)
{
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead;

    //Use the ClearCommError function to get status info on the serial port
    ClearCommError(this->hserial, &this->errors, &this->status);

    //Check if there is something to read
    if(status.cbInQue>0)
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.
        if(status.cbInQue>nbChar)
        {
            toRead = nbChar;
        }
        else
        {
            toRead = status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success
        if(ReadFile(hserial, buffer, toRead, &bytesRead, NULL))
        {
        	buffer[bytesRead]=0;
            return bytesRead;
        }
        else
		{
			buffer[0]=0;
			return 0;
		}

    }

    //If nothing has been read, or that an error was detected return -1
    return -1;

}


bool serial::WriteData(char *buffer, unsigned int nbChar)
{
    DWORD bytesSend;

    //Try to write the buffer on the serial port
    if(!WriteFile(this->hserial, (void *)buffer, nbChar, &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hserial, &this->errors, &this->status);

        return false;
    }
    else
        return true;
}

bool serial::IsConnected()
{
    //Simply return the connection status
    return this->connected;
}
string serial::read()
{
#define DATA_LENGTH 1
	char incomingData[DATA_LENGTH] = "";
	if(IsConnected())
	{
		string line;
		while(true)
		{
			memset(incomingData,0,DATA_LENGTH);
			ReadData(incomingData,DATA_LENGTH);
			if(incomingData[0]=='\0')
			{
				Sleep(100);
			}
			string single(incomingData);
			line.append(single);
			if(isspace(single.c_str()[0]))
			{
				return line;
			}
			single.clear();
		}
	} else throw sserialException("serial: not connected");
}
void serial::write(string &data)
{
	char* inbuf=new char[data.size()+1];
	strcpy(inbuf,data.c_str());
	WriteData(inbuf,data.size());
	delete[] inbuf;
}
void serial::callback(void (function_name)(string&,void*), void* param)
{
	callbackFunc=function_name;
	callbackParam=param;
	callbackEnabled=true;
	thread.create(callbackRun,this);
}
void serial::callbackRun(void* SPVoid)
{
	serial* SP=(serial*)SPVoid;
	while(SP->IsConnected() && SP->callbackEnabled)
	{
		string data=SP->read();
		SP->callbackFunc(data,SP->callbackParam);
	}
}
void serial::clearCallback()
{
	callbackEnabled=false;
}
