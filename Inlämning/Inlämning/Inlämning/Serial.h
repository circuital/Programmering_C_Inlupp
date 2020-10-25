#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
	HANDLE handler;
	bool connected;
	COMSTAT status;
	DWORD errors;
}SERIALPORT;

SERIALPORT SerialInit(char* portName);
int SerialReadPort(SERIALPORT port, char* buffer, unsigned int buf_size);
bool SerialWritePort(SERIALPORT port, char* buffer, unsigned int buf_size);
bool SerialIsConnected(SERIALPORT port);
void SerialClose(SERIALPORT port);