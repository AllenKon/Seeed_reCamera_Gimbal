#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <windows.h>
#include <vector>
#include <sstream>

using namespace std;

int calculateChecksum(uint8_t byteArray[], int start, int end);
bool isSerialPortOpen(HANDLE& hSerial, const char* portName);
void closeSerialPort(HANDLE hSerial);
bool sendData(HANDLE hSerial, const uint8_t* data, size_t length);
bool receiveData(HANDLE hSerial, char* buffer, int length, int timeout);
char ConvertHexChar(char ch);
void StringToHex(const string& str, vector<uint8_t>& senddata);

#endif // SERIAL_COMM_H
