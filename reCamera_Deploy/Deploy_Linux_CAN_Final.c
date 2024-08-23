#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>

int calculateChecksum(uint8_t byteArray[], int start, int end) {
    int sum = 0;
    for (int i = start; i <= end; i++) {
        sum += byteArray[i];
    }
    while (sum > 0xffff)
        sum = (sum & 0xffff) + (sum >> 16);
    return sum;
}

void convertValue(uint32_t value, uint8_t* byteArray, int n) {
    uint32_t shiftedValue = value << 21;
    byteArray[n] = (shiftedValue >> 24) & 0xFF;
    byteArray[n+1] = (shiftedValue >> 16) & 0xFF;
    byteArray[n+2] = (shiftedValue >> 8) & 0xFF;
    byteArray[n+3] = shiftedValue & 0xFF;
}

int isSerialPortOpen(int* serialPort, const char* portName) {
    *serialPort = open(portName, O_RDWR | O_NOCTTY | O_NONBLOCK);  // 设置为非阻塞模式
    if (*serialPort == -1) {
        fprintf(stderr, "Error opening serial port %s\n", portName);
        return 0;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(*serialPort, &tty) != 0) {
        fprintf(stderr, "Error getting serial port state\n");
        close(*serialPort);
        return 0;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(ICRNL | INLCR);

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    tty.c_cc[VTIME] = 1;
    tty.c_cc[VMIN] = 0;

    if (tcsetattr(*serialPort, TCSANOW, &tty) != 0) {
        fprintf(stderr, "Error setting serial port state\n");
        close(*serialPort);
        return 0;
    }

    return 1;
}

void closeSerialPort(int serialPort) {
    close(serialPort);
}

int sendData(int serialPort, const uint8_t* data, size_t length) {
    ssize_t totalBytesWritten = 0;
    fd_set writefds;
    struct timeval tv;

    while (totalBytesWritten < length) {
        FD_ZERO(&writefds);
        FD_SET(serialPort, &writefds);

        tv.tv_sec = 2;
        tv.tv_usec = 0;

        int result = select(serialPort + 1, NULL, &writefds, NULL, &tv);
        if (result < 0) {
            fprintf(stderr, "Error selecting on serial port\n");
            return 0;
        } else if (result == 0) {
            fprintf(stderr, "Serial port write timeout\n");
            return 0;
        }

        if (FD_ISSET(serialPort, &writefds)) {
            ssize_t bytesWritten = write(serialPort, data + totalBytesWritten, length - totalBytesWritten);
            if (bytesWritten == -1) {
                fprintf(stderr, "Error writing to serial port\n");
                return 0;
            }
            totalBytesWritten += bytesWritten;
        }
    }

    return 1;
}

int receiveData(int serialPort, char* buffer, int length, int timeout) {
    tcflush(serialPort, TCIFLUSH);
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(serialPort, &readfds);

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    int result = select(serialPort + 1, &readfds, NULL, NULL, &tv);
    if (result < 0) {
        fprintf(stderr, "Error selecting on serial port\n");
        return 0;
    } else if (result == 0) {
        return 0; // Timeout
    }

    ssize_t bytesRead = read(serialPort, buffer, length);
    if (bytesRead == -1) {
        fprintf(stderr, "Error reading from serial port\n");
        return 0;
    }

    buffer[bytesRead] = '\0';
    return bytesRead > 0;
}

char ConvertHexChar(char ch) {
    if ((ch >= '0') && (ch <= '9'))
        return ch - 0x30; 
    else if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    else if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    else return -1;
}

void String2Hex(const char* str, uint8_t* senddata, size_t* senddataLen) {
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = strlen(str);
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
    *senddataLen = hexdatalen;
}

int sendATCommand(int serialPort, const char* command, int timeout) {
    printf("Sending AT command: %s\n", command);

    // 清理串口缓存
    tcflush(serialPort, TCIFLUSH);

    // 发送AT指令
    if (!sendData(serialPort, (const uint8_t*)command, strlen(command))) {
        fprintf(stderr, "Failed to send AT command.\n");
        return 0;
    }


    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <angle1> [<angle2> ... <angleN>]\n", argv[0]);
        return 1;
    }

    uint8_t motorIDs[2] = {1, 2};
    int motorIDCount = 2;
    int serialPort;
    double angles[17];
    int angleCount = 0;
    int spds[2] = {300, 300};
    const char* portName = "/dev/ttyS0";
    char buffer[256];

    while (1) {
        if (isSerialPortOpen(&serialPort, portName)) {
            break;
        } else {
            printf("Serial port %s is not open.\n", portName);
        }
        sleep(2);  // 每隔2秒检测一次
    }
    
    while (!sendATCommand(serialPort, "AT+AT\r\n", 2000)) {
        printf("AT command not acknowledged, retrying...\n");
        sleep(1);  // 每隔2秒重试一次
    }
    tcflush(serialPort, TCIFLUSH);

    
    // 解析传入的角度值
    int validInput = 1;
    for (int i = 1; i < argc; ++i) {
        double angle = atof(argv[i]);
        if (angle >= 0 && angle <= 360) {
            angles[angleCount++] = angle;
        } else {
            fprintf(stderr, "角度值必须在0到360之间。\n");
            validInput = 0;
            break;
        }
    }

    if (!validInput) {
        return 1;
    }

    if (motorIDCount != angleCount) {
        fprintf(stderr, "电机ID数量和角度值数量不匹配。\n");
        return 1;
    }

    for (int i = 0; i < motorIDCount; ++i) {
        uint8_t motorID = motorIDs[i];
        uint16_t spd = spds[i];
        uint8_t byteArray[4];
        uint64_t pos = (uint64_t)(angles[i] * 100);
        uint8_t hexArray[17] = {0x41, 0x54, 0x28, 0x20, 0x00, 0x00, 0x08, 0xA4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0a};
        uint16_t canID_send;

        canID_send = 0x140 + motorID;
        convertValue(canID_send, hexArray, 2);

        for (int j = 0; j < 2; ++j) {
            hexArray[9 + j] = *((uint8_t*)(&spd) + j);
        }

        for (int j = 0; j < 4; ++j) {
            hexArray[11 + j] = *((uint8_t*)(&pos) + j);
        }

        char hexString[36];
        for (int k = 0; k < 17; ++k) {
            sprintf(&hexString[k * 2], "%02X", hexArray[k]);
        }
        hexString[34] = '\r';
        hexString[35] = '\n';  
        printf("Data to send: %s\n", hexString);

        uint8_t dataToSend[20];
        size_t dataToSendLen;
        String2Hex(hexString, dataToSend, &dataToSendLen);

        if (isSerialPortOpen(&serialPort, portName)) {
            if (sendData(serialPort, dataToSend, dataToSendLen)) {
                printf("Data sent successfully to motor ID %d.\n", motorID);
                if (receiveData(serialPort, buffer, sizeof(buffer) - 1, 50)) {
                    printf("Received data: %s\n", buffer);
                } else {
                    printf("No data received within timeout period.\n");
                }
            } else {
                fprintf(stderr, "Failed to send data to motor ID %d.\n", motorID);
            }
            closeSerialPort(serialPort);
        } else {
            fprintf(stderr, "Failed to reopen serial port for motor ID %d.\n", motorID);
        }
    }

    closeSerialPort(serialPort);
    return 0;
}
