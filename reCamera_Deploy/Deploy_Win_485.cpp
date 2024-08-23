#include <thread>
#include <chrono>
#include "Deploy.h"

using namespace std;

int calculateChecksum(uint8_t [], int, int);

//Calculate the checksum
int calculateChecksum(uint8_t byteArray[], int start, int end) {
    int sum = 0;
    for (int i = start; i <= end; i++) {
        sum += byteArray[i];
    }
    while (sum > 0xffff)
        sum = (sum & 0xffff)+(sum>>16);
    return sum;
}

bool isSerialPortOpen(HANDLE& hSerial, const char* portName) {
    hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            cerr << "Error: Serial port " << portName << " not found." << endl;
        } else {
            cerr << "Error opening serial port " << portName << endl;
        }
        return false;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error getting serial port state" << endl;
        CloseHandle(hSerial);
        return false;
    }

    dcbSerialParams.BaudRate = CBR_115200;  // Set the Baud rate to 115200
    dcbSerialParams.ByteSize = 8;   // Set the data byte size to 8 bits
    dcbSerialParams.StopBits = ONESTOPBIT;  // Set the number of stop bits to 1, ONESTOPBIT (1), ONE5STOPBITS (1.5), or TWOSTOPBITS (2)
    dcbSerialParams.Parity = NOPARITY;  // Set parity to none, NOPARITY (0), ODDPARITY (1), EVENPARITY (2), MARKPARITY (3), or SPACEPARITY (4)
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error setting serial port state" << endl;
        CloseHandle(hSerial);
        return false;
    }

    return true;
}

void closeSerialPort(HANDLE hSerial) {
    CloseHandle(hSerial);
}

bool sendData(HANDLE hSerial, const uint8_t* data, size_t length) {
    DWORD bytesWritten;
    if (!WriteFile(hSerial, data, length, &bytesWritten, NULL)) {
        cerr << "Error writing to serial port" << endl;
        return false;
    }
    return true;
}

bool receiveData(HANDLE hSerial, char* buffer, int length, int timeout) {
    DWORD bytesRead;
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = timeout;
    timeouts.ReadTotalTimeoutConstant = timeout;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    SetCommTimeouts(hSerial, &timeouts);

    if (!ReadFile(hSerial, buffer, length, &bytesRead, NULL)) {
        cerr << "Error reading from serial port" << endl;
        return false;
    }
    buffer[bytesRead] = '\0'; // Add string terminator
    return bytesRead > 0;
}

// Convert characters to true hexadecimal
char ConvertHexChar(char ch) {
    if ((ch >= '0') && (ch <= '9'))
        return ch - 0x30;
    else if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    else if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    else return -1;
}

void StringToHex(const string& str, vector<uint8_t>& senddata) {
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len / 2);
    char lstr, hstr;
    for (int i = 0; i < len;) {
        hstr = str[i];
        if (hstr == ' ') {
            i++;
            continue;
        }
        i++;
        if (i >= len)
            break;
        lstr = str[i];
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if ((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

int main() {
    vector<uint8_t> motorIDs;
    HANDLE hSerial;
    vector<double> angles;
    const char* portName = "COM10";
    
    while (true) {
        if (isSerialPortOpen(hSerial, portName)) {
            cout << "Serial port " << portName << " is open." << endl;
            break;
        } else {
            cout << "Serial port " << portName << " is not open." << endl;
        }
        this_thread::sleep_for(chrono::seconds(2));  // 每隔2秒检测一次
    }
    
    cout << "Please enter the IDs of the motors to be operated (separated by spaces, range 0-16): ";
    string input;
    getline(cin, input);
    stringstream ss(input);
    int id;
    while (ss >> id) {
        if (id >= 0 && id <= 16) {
            motorIDs.push_back(static_cast<uint8_t>(id));
        } else {
            cerr << "The motor ID must be between 0 and 16." << endl;
            return -1;
        }
    }
    char buffer[256]; // 假设接收的数据不超过256字节

    while (true) {
        angles.clear();
        cout << "Enter the rotation angle (0-360°, separated by spaces, corresponding to different motor IDs): ";
        getline(cin, input);
        ss.clear();
        ss.str(input);
        double angle;
        bool validInput = true;
        while (ss >> angle) {
            if (angle >= 0 && angle <= 360) {
                angles.push_back(angle);
            } else {
                cerr << "The angle value must be between 0 and 360." << endl;
                validInput = false;
                break;
            }
        }

        if (!validInput) {
            continue; // 重新输入角度值
        }

        if (motorIDs.size() != angles.size()) {
            cerr << "There is a mismatch between the number of motor IDs and the number of angle values." << endl;
            continue; // 重新输入
        }

        for (size_t i = 0; i < motorIDs.size(); ++i) {
            uint8_t motorID = motorIDs[i];
            uint64_t pos = static_cast<uint64_t>(angles[i] * 100);
            uint8_t hexArray[14] = {0x3E, 0xA3, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //Refer to MS_Motor_RS485_Instruction if the command needs to be changed.
            hexArray[2] = motorID;
            hexArray[4] = calculateChecksum(hexArray, 0, 3);

            for (int j = 0; j < 8; ++j) {
                hexArray[5 + j] = *((uint8_t*)(&pos) + j);
            }
            hexArray[13] = calculateChecksum(hexArray, 5, 12);

            // 将hexArray整合成连续的字符串
            stringstream hexStringStream;
            for (int k = 0; k < 14; ++k) {
                hexStringStream << uppercase << hex << setw(2) << setfill('0') << (int)hexArray[k];
            }
            hexStringStream << '\n';  // 添加新行符
            string hexString = hexStringStream.str();
            cout << "Data to send: " << hexString << endl;

            // 转换为实际的十六进制字节数组
            vector<uint8_t> dataToSend;
            StringToHex(hexString, dataToSend);

            if (sendData(hSerial, dataToSend.data(), dataToSend.size())) {
                cout << "Data sent successfully to motor ID " << (int)motorID << "." << endl;
                if (receiveData(hSerial, buffer, sizeof(buffer) - 1, 500)) { // 超时时间500ms
                    // 将接收到的数据转换为字符串并输出
                    cout << "Received data: " << buffer << endl;
                } else {
                    cout << "No data received within timeout period." << endl;
                }
            } else {
                cerr << "Failed to send data to motor ID " << (int)motorID << "." << endl;
            }
        }
    }

    closeSerialPort(hSerial);
    return 0;
}
